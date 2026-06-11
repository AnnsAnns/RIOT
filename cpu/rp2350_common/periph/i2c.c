/*
 * SPDX-FileCopyrightText: 2026 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           I2C implementation for the RP2350
 *
 * The RP2350 features two Synopsys DW_apb_i2c blocks clocked by clk_sys.
 * This driver operates them as polling I2C masters, the transfer sequence
 * follows the one used by the pico-sdk. Only 7 bit addressing is supported.
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include <errno.h>

#include "mutex.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#include "periph/i2c.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static mutex_t locks[I2C_NUMOF];

/* true if the previous transfer was ended without a STOP condition and the
 * next transfer therefore has to issue a repeated START */
static bool restart_on_next[I2C_NUMOF];

static void _init_pins(i2c_t dev)
{
    const gpio_t pins[] = { i2c_config[dev].sda_pin, i2c_config[dev].scl_pin };

    for (unsigned i = 0; i < 2; i++) {
        /* Set the pin to the I2C function */
        *calculate_gpio_io_ctrl_register_addr(pins[i]) = FUNCTION_SELECT_I2C;
        /* Enable input and the internal pull-up, I2C is an open-drain bus */
        atomic_set(calculate_gpio_pad_register_addr(pins[i]),
                   PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_PUE_BITS);
        /* Clear the ISO bits and disable the pull-down */
        atomic_clear(calculate_gpio_pad_register_addr(pins[i]),
                     PADS_BANK0_ISO_BITS | PADS_BANK0_GPIO0_PDE_BITS);
    }
}

static void _set_baudrate(I2C0_Type *i2c, uint32_t speed)
{
    /* The I2C blocks are clocked by clk_sys */
    uint32_t freq_in = CPUFREQ;
    uint32_t period = (freq_in + (speed / 2)) / speed;
    /* SCL spends 60 % of the period low, as tLOW > tHIGH is demanded for
     * both fast mode and fast mode plus */
    uint32_t lcnt = (period * 3) / 5;
    uint32_t hcnt = period - lcnt;

    assert((hcnt >= 8) && (hcnt <= UINT16_MAX));
    assert((lcnt >= 8) && (lcnt <= UINT16_MAX));

    /* Per I2C specification: SDA hold time of 300 ns for standard and fast
     * mode, 120 ns for fast mode plus */
    uint32_t sda_tx_hold_count;
    if (speed < I2C_SPEED_FAST_PLUS) {
        sda_tx_hold_count = ((freq_in * 3) / 10000000u) + 1;
    }
    else {
        sda_tx_hold_count = ((freq_in * 3) / 25000000u) + 1;
    }
    assert(sda_tx_hold_count <= (lcnt - 2));

    i2c->IC_FS_SCL_HCNT = hcnt;
    i2c->IC_FS_SCL_LCNT = lcnt;
    i2c->IC_FS_SPKLEN = (lcnt < 16) ? 1 : (lcnt / 16);
    i2c->IC_SDA_HOLD = (i2c->IC_SDA_HOLD & ~I2C_IC_SDA_HOLD_IC_SDA_TX_HOLD_BITS)
                     | sda_tx_hold_count;
}

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    I2C0_Type *i2c = i2c_config[dev].dev;

    mutex_init(&locks[dev]);
    restart_on_next[dev] = false;

    /* High speed mode (3.4 MHz) is not supported by the hardware */
    assert(i2c_config[dev].speed <= I2C_SPEED_FAST_PLUS);

    /* Reset the I2C block, so we can be sure it is in a known state */
    uint32_t reset_bit = (i2c == I2C0) ? RESET_I2C0 : RESET_I2C1;
    reset_component(reset_bit, reset_bit);

    /* The configuration registers can only be written while disabled */
    i2c->IC_ENABLE = 0;
    i2c->IC_CON = I2C_IC_CON_MASTER_MODE_BITS | I2C_IC_CON_SPEED_FAST_BITS |
                  I2C_IC_CON_IC_RESTART_EN_BITS |
                  I2C_IC_CON_IC_SLAVE_DISABLE_BITS |
                  I2C_IC_CON_TX_EMPTY_CTRL_BITS;
    /* We poll the raw status, so no interrupts are needed */
    i2c->IC_INTR_MASK = 0;
    /* Assert the FIFO status flags already for a single byte */
    i2c->IC_TX_TL = 0;
    i2c->IC_RX_TL = 0;

    _set_baudrate(i2c, i2c_config[dev].speed);
    _init_pins(dev);
}

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_lock(&locks[dev]);
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_unlock(&locks[dev]);
}

static int _abort_to_errno(uint32_t abort_reason)
{
    if (abort_reason & I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_BITS) {
        return -ENXIO;
    }
    if (abort_reason & I2C_IC_TX_ABRT_SOURCE_ARB_LOST_BITS) {
        return -EAGAIN;
    }
    return -EIO;
}

/**
 * @brief   Prepare a transfer and decide whether a repeated START is needed
 *
 * @return  1 if the first byte of the transfer must issue a repeated START,
 *          0 if not, negative errno on error
 */
static int _setup_transfer(i2c_t dev, uint16_t addr, uint8_t flags)
{
    I2C0_Type *i2c = i2c_config[dev].dev;

    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }

    if (flags & I2C_NOSTART) {
        /* Without a START we can only continue a still ongoing transfer to
         * the same target, in which case the block is already enabled */
        if (!(i2c->IC_ENABLE & I2C_IC_ENABLE_ENABLE_BITS)) {
            return -EOPNOTSUPP;
        }
        return 0;
    }

    /* The target address can only be set while the block is disabled */
    i2c->IC_ENABLE = 0;
    i2c->IC_TAR = addr;
    i2c->IC_ENABLE = I2C_IC_ENABLE_ENABLE_BITS;

    return restart_on_next[dev] ? 1 : 0;
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data,
                   size_t len, uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    I2C0_Type *i2c = i2c_config[dev].dev;
    uint8_t *buf = data;
    bool nostop = flags & I2C_NOSTOP;
    uint32_t abort_reason = 0;
    bool abort = false;

    if ((len == 0) || (data == NULL)) {
        return -EINVAL;
    }

    int restart = _setup_transfer(dev, addr, flags);
    if (restart < 0) {
        return restart;
    }

    for (size_t i = 0; i < len; i++) {
        bool first = (i == 0);
        bool last = (i == (len - 1));

        /* Wait for room in the command FIFO */
        while ((I2C_FIFO_DEPTH - i2c->IC_TXFLR) == 0) {}

        i2c->IC_DATA_CMD = ((first && restart) ? I2C_IC_DATA_CMD_RESTART_BITS : 0)
                         | ((last && !nostop) ? I2C_IC_DATA_CMD_STOP_BITS : 0)
                         | I2C_IC_DATA_CMD_CMD_BITS;

        do {
            abort_reason = i2c->IC_TX_ABRT_SOURCE;
            if (i2c->IC_RAW_INTR_STAT & I2C_IC_RAW_INTR_STAT_TX_ABRT_BITS) {
                /* Reading IC_CLR_TX_ABRT clears the flag and the reason */
                (void)i2c->IC_CLR_TX_ABRT;
                abort = true;
            }
        } while (!abort && (i2c->IC_RXFLR == 0));

        if (abort) {
            break;
        }

        buf[i] = (uint8_t)i2c->IC_DATA_CMD;
    }

    restart_on_next[dev] = nostop && !abort;

    if (abort) {
        DEBUG("[i2c] read: transfer aborted, reason 0x%08x\n",
              (unsigned)abort_reason);
        return _abort_to_errno(abort_reason);
    }

    return 0;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                    size_t len, uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    I2C0_Type *i2c = i2c_config[dev].dev;
    const uint8_t *buf = data;
    bool nostop = flags & I2C_NOSTOP;
    uint32_t abort_reason = 0;
    bool abort = false;

    if ((len == 0) || (data == NULL)) {
        return -EINVAL;
    }

    int restart = _setup_transfer(dev, addr, flags);
    if (restart < 0) {
        return restart;
    }

    for (size_t i = 0; i < len; i++) {
        bool first = (i == 0);
        bool last = (i == (len - 1));

        i2c->IC_DATA_CMD = ((first && restart) ? I2C_IC_DATA_CMD_RESTART_BITS : 0)
                         | ((last && !nostop) ? I2C_IC_DATA_CMD_STOP_BITS : 0)
                         | buf[i];

        /* Wait until the address and data left the TX shift register */
        while (!(i2c->IC_RAW_INTR_STAT & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS)) {}

        abort_reason = i2c->IC_TX_ABRT_SOURCE;
        if (abort_reason) {
            /* Reading IC_CLR_TX_ABRT clears the flag and the reason */
            (void)i2c->IC_CLR_TX_ABRT;
            abort = true;
        }

        if (abort || (last && !nostop)) {
            /* Wait for the STOP condition, which the hardware also
             * generates when a transfer is aborted */
            while (!(i2c->IC_RAW_INTR_STAT & I2C_IC_RAW_INTR_STAT_STOP_DET_BITS)) {}
            (void)i2c->IC_CLR_STOP_DET;
        }

        if (abort) {
            break;
        }
    }

    restart_on_next[dev] = nostop && !abort;

    if (abort) {
        DEBUG("[i2c] write: transfer aborted, reason 0x%08x\n",
              (unsigned)abort_reason);
        return _abort_to_errno(abort_reason);
    }

    return 0;
}

/** @} */
