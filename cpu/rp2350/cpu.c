/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include "cpu.h"
#include "kernel_init.h"
#include "macros/units.h"
#include "stdio_base.h"

#define ENABLE_DEBUG        0
#include "debug.h"

static void _cpu_reset(void)
{
}

/**
 * @brief Configure the clock system
 */
 static void cpu_clock_init(void)
 {
 }

#define LED_PIN 25

#define pico_default_asm_volatile(...) __asm volatile (".syntax unified\n" __VA_ARGS__)

// ----------------------------------------------------------------------------
// Single-bit write instructions

// Write a 1-bit value to any output. Equivalent to:
//
//     if (val)
//         gpioc_hilo_out_set(1ull << pin);
//     else
//         gpioc_hilo_out_clr(1ull << pin);
__always_inline static void gpioc_bit_out_put(int pin, bool val) {
    pico_default_asm_volatile ("mcrr p0, #4, %0, %1, c0" : : "r" (pin), "r" (val));
}

void cpu_init(void)
{
    cpu_clock_init();

    /* initialize the Cortex-M core */
    cortexm_init();

    _cpu_reset();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    gpioc_bit_out_put(LED_PIN, 1);

    // This will cause our program to never return
    // but also means that we can be sure nothing else is running
    while (1){}
}
