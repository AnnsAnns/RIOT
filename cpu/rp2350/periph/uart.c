/*
 * Copyright (C) 2024 RIOT Contributors
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rp2350
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       UART driver implementation for the RP2350
 *
 * @author      RIOT Contributors
 *
 * @}
 */

#include "assert.h"
#include "bitarithm.h"
#include "board.h"
#include "io_reg.h"
#include "irq.h"
#include "mutex.h"
#include "RP2350.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG    0
#include "debug.h"

/**
 * @brief   UART device configurations
 */
static uart_isr_ctx_t ctx[UART_NUMOF];

/* back up values of registers used during uart_poweroff() / uart_poweron() */
static uint32_t uartibrd;
static uint32_t uartfbrd;
static uint32_t uartlcr_h;
static uint32_t uartcr;

/**
 * @brief   Enable UART interrupt for RX
 *
 * @param[in] uart      UART device to enable interrupts for
 */
static void _irq_enable(uart_t uart)
{
    UART0_Type *dev = uart_config[uart].dev;
    dev->UARTIMSC = UART0_UARTIMSC_RXIM_Msk;
    NVIC_EnableIRQ(uart_config[uart].irqn);
}

/**
 * @brief   Set UART symbol rate
 *
 * @param[in] uart      UART device
 * @param[in] baud      Desired baud rate
 */
static void _set_symbolrate(uart_t uart, uint32_t baud)
{
    assert(baud != 0);
    UART0_Type *dev = uart_config[uart].dev;
    uint32_t baud_rate_div = (8 * CLOCK_PERIPH / baud);
    uint32_t baud_ibrd = baud_rate_div >> 7;
    uint32_t baud_fbrd;

    if (baud_ibrd == 0) {
        baud_ibrd = 1;
        baud_fbrd = 0;
    }
    else if (baud_ibrd >= 65535) {
        baud_ibrd = 65535;
        baud_fbrd = 0;
    }
    else {
        baud_fbrd = ((baud_rate_div & 0x7f) + 1) / 2;
    }

    dev->UARTIBRD = baud_ibrd;
    dev->UARTFBRD = baud_fbrd;
}

/**
 * @brief   Reset and enable UART peripheral
 *
 * @param[in] uart      UART device to power on
 */
static void _poweron(uart_t uart)
{
    uint32_t reset_bit_mask = (uart) ? RESETS_RESET_uart1_Msk : RESETS_RESET_uart0_Msk;
    periph_reset(reset_bit_mask);
    periph_reset_done(reset_bit_mask);
}

int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t uart_parity,
              uart_stop_bits_t stop_bits)
{
    assert((unsigned)uart < UART_NUMOF);
    UART0_Type *dev = uart_config[uart].dev;

    io_reg_atomic_clear(&dev->UARTCR,
                        UART0_UARTCR_UARTEN_Msk | UART0_UARTCR_TXE_Msk | UART0_UARTCR_RXE_Msk);

    /* Set data bits */
    dev->UARTLCR_H = (uint32_t)data_bits << UART0_UARTLCR_H_WLEN_Pos;

    /* Set stop bits */
    if (stop_bits == UART_STOP_BITS_2) {
        io_reg_atomic_set(&dev->UARTLCR_H, UART0_UARTLCR_H_STP2_Msk);
    }

    /* Set parity mode */
    switch (uart_parity) {
    case UART_PARITY_NONE:
        break;
    case UART_PARITY_EVEN:
        io_reg_atomic_set(&dev->UARTLCR_H, UART0_UARTLCR_H_EPS_Msk | UART0_UARTLCR_H_PEN_Msk);
        break;
    case UART_PARITY_ODD:
        io_reg_atomic_set(&dev->UARTLCR_H, UART0_UARTLCR_H_PEN_Msk);
        break;
    default:
        return UART_NOMODE;
    }

    /* Enable UART, transmitter and receiver */
    io_reg_atomic_set(&dev->UARTCR,
                      UART0_UARTCR_UARTEN_Msk | UART0_UARTCR_TXE_Msk | UART0_UARTCR_RXE_Msk);

    return UART_OK;
}

void uart_init_pins(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    
    /* Configure TX pin */
    /* Set pad control register for TX pin */
    gpio_pad_ctrl_t tx_pad_config = {
        .drive_strength = DRIVE_STRENGTH_2MA,
        .output_disable = 0,
        .slew_rate_fast = 0,
    };
    
    /* Configure RX pin if needed */
    if (ctx[uart].rx_cb) {
        gpio_pad_ctrl_t rx_pad_config = {
            .pull_up_enable = 1,
            .input_enable = 1,
            .schmitt_trig_enable = 1,
        };
        
        /* Connect RX pin to UART function */
        IO_BANK0->GPIO_CTRL[uart_config[uart].rx_pin].CTRL = (2 << 0); /* Function 2 is UART */
        
        /* Set pad control registers */
        PADS_BANK0->GPIO[uart_config[uart].rx_pin] = 
            (rx_pad_config.pull_up_enable << 3) |
            (rx_pad_config.input_enable << 6) |
            (rx_pad_config.schmitt_trig_enable << 1);
    }
    
    /* Connect TX pin to UART function */
    IO_BANK0->GPIO_CTRL[uart_config[uart].tx_pin].CTRL = (2 << 0); /* Function 2 is UART */
    
    /* Set pad control registers */
    PADS_BANK0->GPIO[uart_config[uart].tx_pin] = 
        (tx_pad_config.drive_strength << 4) |
        (tx_pad_config.slew_rate_fast << 0);
}

void uart_deinit_pins(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    
    /* Reset TX pin configuration */
    IO_BANK0->GPIO_CTRL[uart_config[uart].tx_pin].CTRL = 0; /* Function 0 is SIO (GPIO) */
    PADS_BANK0->GPIO[uart_config[uart].tx_pin] = 0;
    SIO->GPIO_OE_CLR = 1UL << uart_config[uart].tx_pin;
    
    /* Reset RX pin configuration if needed */
    if (ctx[uart].rx_cb) {
        IO_BANK0->GPIO_CTRL[uart_config[uart].rx_pin].CTRL = 0; /* Function 0 is SIO (GPIO) */
        PADS_BANK0->GPIO[uart_config[uart].rx_pin] = 0;
    }
}

void uart_poweron(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    _poweron(uart);
    UART0_Type *dev = uart_config[uart].dev;
    
    /* Restore configuration registers */
    dev->UARTIBRD = uartibrd;
    dev->UARTFBRD = uartfbrd;
    dev->UARTLCR_H = uartlcr_h;
    dev->UARTCR = uartcr;
    
    /* Restore IRQs if needed */
    if (ctx[uart].rx_cb != NULL) {
        _irq_enable(uart);
    }
    
    uart_init_pins(uart);
}

void uart_poweroff(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    UART0_Type *dev = uart_config[uart].dev;
    
    /* Backup configuration registers */
    uartibrd = dev->UARTIBRD;
    uartfbrd = dev->UARTFBRD;
    uartlcr_h = dev->UARTLCR_H;
    uartcr = dev->UARTCR;
    
    /* Disconnect GPIOs and power off peripheral */
    uart_deinit_pins(uart);
    periph_reset((uart) ? RESETS_RESET_uart1_Msk : RESETS_RESET_uart0_Msk);
}

int uart_init(uart_t uart, uint32_t baud, uart_rx_cb_t rx_cb, void *arg)
{
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    UART0_Type *dev = uart_config[uart].dev;
    ctx[uart].rx_cb = rx_cb;
    ctx[uart].arg = arg;

    _poweron(uart);
    uart_init_pins(uart);

    /* Set baud rate */
    _set_symbolrate(uart, baud);

    /* Configure UART mode: 8N1 by default */
    if (uart_mode(uart, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1) != UART_OK) {
        return UART_NOMODE;
    }

    /* Enable RX and IRQs if callback is provided */
    if (rx_cb != NULL) {
        _irq_enable(uart);
        
        /* Clear any pending data and IRQ to avoid receiving garbage */
        uint32_t status = dev->UARTRIS;
        dev->UARTICR = status;
        (void)dev->UARTDR;
        io_reg_atomic_set(&dev->UARTCR, UART0_UARTCR_RXE_Msk);
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert((unsigned)uart < UART_NUMOF);
    UART0_Type *dev = uart_config[uart].dev;

    for (size_t i = 0; i < len; i++) {
        /* Wait for TX FIFO to have space */
        while (dev->UARTFR & UART0_UARTFR_TXFF_Msk) { }
        
        /* Write data */
        dev->UARTDR = data[i];
        
        /* Wait for TX to complete */
        while (!(dev->UARTRIS & UART0_UARTRIS_TXRIS_Msk)) { }
    }
}

#ifdef MODULE_PERIPH_UART_RECONFIGURE
gpio_t uart_pin_rx(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    return uart_config[uart].rx_pin;
}

gpio_t uart_pin_tx(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    return uart_config[uart].tx_pin;
}
#endif

/**
 * @brief Common UART IRQ handler
 *
 * @param[in] num       UART device number
 */
static void isr_handler(uint8_t num)
{
    UART0_Type *dev = uart_config[num].dev;

    uint32_t status = dev->UARTMIS;
    dev->UARTICR = status;

    if (status & UART0_UARTMIS_RXMIS_Msk) {
        uint32_t data = dev->UARTDR;
        if (data & (UART0_UARTDR_BE_Msk | UART0_UARTDR_PE_Msk | UART0_UARTDR_FE_Msk)) {
            DEBUG_PUTS("[rp2350] UART RX error (parity, break, or framing error)");
        }
        else {
            ctx[num].rx_cb(ctx[num].arg, (uint8_t)data);
        }
    }
}

/**
 * @brief UART0 IRQ handler
 */
void isr_uart0(void)
{
    isr_handler(0);
    cortexm_isr_end();
}

/**
 * @brief UART1 IRQ handler
 */
void isr_uart1(void)
{
    isr_handler(1);
    cortexm_isr_end();
}
