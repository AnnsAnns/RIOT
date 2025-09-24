/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "clock_conf.h"
#include "kernel_init.h"
#include "periph/init.h"
#include "periph/uart.h"
#include "periph_conf.h"

#include "stdio_uart.h"
#include <stdio.h>
#include <sys/unistd.h>

void gpio_reset(void)
{
    reset_component(RESET_PADS_BANK0, RESET_PADS_BANK0);
    reset_component(RESET_IO_BANK0, RESET_IO_BANK0);
}

void enable_irq(uint32_t irq_no)
{
    uint32_t index = irq_no / 16;
    uint32_t mask = 1u << (irq_no % 16);

    __asm__ volatile(
        "csrs 0xbe0, %0\n"
        : : "r"(index | (mask << 16))

    );
}

void force_interrupt(uint32_t irq_no)
{
    uint32_t index = irq_no / 16;
    uint32_t mask = 1u << (irq_no % 16);

    __asm__ volatile(
        "csrs 0xbe2, %0\n"
        : : "r"(index | (mask << 16))
    );
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks, peripheral
 */
void cpu_init(void)
{
    riscv_init();

    uint32_t ra_register = 0;

    __asm__ volatile ("mv %0, ra" : "=r"(ra_register));

    /* Reset GPIO state */
    gpio_reset();

    /* Reset clock to default state */
    clock_reset();

    /* initialize the CPU clock */
    cpu_clock_init();

    /* initialize the early peripherals */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();

    board_init();

    xosc_sleep(1000);
    printf("Enabling IRQ 50\n");

    enable_irq(51);
    //
    // printf("Interrupt force\n");

    printf("ra register at start of cpu_init: 0x%lx\n", ra_register);

    printf("Send non-enabled interrupt 50\n");
    force_interrupt(50);
    printf("Send enabled interrupt 51\n");
    force_interrupt(51);
    printf("Debugger halt message haha\n");

    printf("Back from interrupt :D\n");

    __asm__ volatile ("nop");
}
