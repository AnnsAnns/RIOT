/*
 * SPDX-FileCopyrightText: 2026 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           I2C configuration for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "RP2350.h"
#include "periph_cpu.h"

/** Master mode enable bit in IC_CON */
#define I2C_IC_CON_MASTER_MODE_BITS (1u << 0u)

/** Fast mode speed selection in IC_CON, used for all supported speeds */
#define I2C_IC_CON_SPEED_FAST_BITS (2u << 1u)

/** Master restart enable bit in IC_CON */
#define I2C_IC_CON_IC_RESTART_EN_BITS (1u << 5u)

/** Slave disable bit in IC_CON */
#define I2C_IC_CON_IC_SLAVE_DISABLE_BITS (1u << 6u)

/** TX_EMPTY asserts only once the shift register is empty as well */
#define I2C_IC_CON_TX_EMPTY_CTRL_BITS (1u << 8u)

/** Read command bit in IC_DATA_CMD */
#define I2C_IC_DATA_CMD_CMD_BITS (1u << 8u)

/** Issue a STOP after this byte in IC_DATA_CMD */
#define I2C_IC_DATA_CMD_STOP_BITS (1u << 9u)

/** Issue a RESTART before this byte in IC_DATA_CMD */
#define I2C_IC_DATA_CMD_RESTART_BITS (1u << 10u)

/** TX FIFO and shift register empty bit in IC_RAW_INTR_STAT */
#define I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS (1u << 4u)

/** Transfer abort bit in IC_RAW_INTR_STAT */
#define I2C_IC_RAW_INTR_STAT_TX_ABRT_BITS (1u << 6u)

/** STOP condition detected bit in IC_RAW_INTR_STAT */
#define I2C_IC_RAW_INTR_STAT_STOP_DET_BITS (1u << 9u)

/** Address byte not ACKed (7 bit mode) in IC_TX_ABRT_SOURCE */
#define I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_BITS (1u << 0u)

/** Data byte not ACKed in IC_TX_ABRT_SOURCE */
#define I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_BITS (1u << 3u)

/** Arbitration lost in IC_TX_ABRT_SOURCE */
#define I2C_IC_TX_ABRT_SOURCE_ARB_LOST_BITS (1u << 12u)

/** Enable bit in IC_ENABLE */
#define I2C_IC_ENABLE_ENABLE_BITS (1u << 0u)

/** Mask of the SDA TX hold time field in IC_SDA_HOLD */
#define I2C_IC_SDA_HOLD_IC_SDA_TX_HOLD_BITS (0xffffu)

/** Depth of the I2C TX and RX FIFOs */
#define I2C_FIFO_DEPTH 16u

/** Reset bit for the I2C0 peripheral */
#define RESET_I2C0 (1u << 4u)

/** Reset bit for the I2C1 peripheral */
#define RESET_I2C1 (1u << 5u)

/** Pull-up enable bit in a pad configuration register */
#define PADS_BANK0_GPIO0_PUE_BITS (1u << 3u)

/** Pull-down enable bit in a pad configuration register */
#define PADS_BANK0_GPIO0_PDE_BITS (1u << 2u)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN
/** Override the I2C speed values with the bus frequency in Hz */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       = 10000u,   /**< low speed mode:    ~10 kbit/s */
    I2C_SPEED_NORMAL    = 100000u,  /**< normal mode:      ~100 kbit/s */
    I2C_SPEED_FAST      = 400000u,  /**< fast mode:        ~400 kbit/s */
    I2C_SPEED_FAST_PLUS = 1000000u, /**< fast plus mode:  ~1000 kbit/s */
    I2C_SPEED_HIGH      = 3400000u, /**< high speed mode: not supported */
} i2c_speed_t;
#endif /* ndef DOXYGEN */

/**
 * @name    Use the common fallback implementations for the register access
 *          functions
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_READ_REGS
#define PERIPH_I2C_NEED_WRITE_REG
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

/**
 * @brief   Configuration details for an I2C interface
 */
typedef struct {
    I2C0_Type *dev;     /**< Base address of the I/O registers of the device */
    gpio_t sda_pin;     /**< GPIO pin to use for SDA */
    gpio_t scl_pin;     /**< GPIO pin to use for SCL */
    i2c_speed_t speed;  /**< Bus speed in Hz */
} i2c_conf_t;

#ifdef __cplusplus
}
#endif

/** @} */
