/*
 * Copyright (C) 2021 Nick Weiler, Justus Krebs, Franz Freitag
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       UART driver implementation for the RP2040
 *
 * @author      Franz Freitag <franz.freitag@st.ovgu.de>
 * @author      Justus Krebs <justus.krebs@st.ovgu.de>
 * @author      Nick Weiler <nick.weiler@st.ovgu.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
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
#include "uart_conf.h"

static uart_isr_ctx_t ctx[UART_NUMOF];

void _disable(uart_t uart) {
    assert((unsigned)uart < UART_NUMOF);
    gpio_reset_all_config(uart_config[uart].tx_pin);
    SIO->GPIO_OE_CLR = 1LU << uart_config[uart].tx_pin;
    if (ctx[uart].rx_cb) {
        gpio_reset_all_config(uart_config[uart].rx_pin);
    }
    
    periph_reset((uart) ? RESETS_RESET_uart1_Msk : RESETS_RESET_uart0_Msk);
}

void _enable(uart_t uart) {
    uint32_t reset_bit_mask = (uart) ? RESETS_RESET_uart1_Msk : RESETS_RESET_uart0_Msk;
    periph_reset(reset_bit_mask);
    periph_reset_done(reset_bit_mask);
}

void uart_init_pins(uart_t uart) {
    
}

int uart_init(uart_t uart, uint32_t baud, uart_rx_cb_t rx_cb, void *arg) {
    // Get the UART configuration for the given UART
    uart_conf_t *uart_conf = &uart_config[uart];
    // Get the UART device
    UART0_Type *dev = uart_conf->dev;
    // Set the RX callback
    ctx[uart].rx_cb = rx_cb;
    // Set the RX callback argument
    ctx[uart].arg = arg;

    // Initialize the UART pins
    // Possibly also power on the UART device here?
    uart_init_pins(uart);

        /* Beware: Changes to the symbol rate only take affect after touching the UARTLCR_H register,
     * which is done in uart_mode(). */
    _set_symbolrate(uart, baud);

    if (uart_mode(uart, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1) != UART_OK) {
        return UART_NOMODE;
    }

    /* enable RX and IRQs, if needed */
    if (rx_cb != NULL) {
        _irq_enable(uart);
        /* clear any pending data and IRQ to avoid receiving a garbage char */
        uint32_t status = dev->UARTRIS;
        dev->UARTICR = status;
        (void)dev->UARTDR;
        io_reg_atomic_set(&dev->UARTCR, UART0_UARTCR_RXE_Msk);
    }

    return UART_OK;
}
