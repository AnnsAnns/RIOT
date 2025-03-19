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
#include "RP2350.h"

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

 __STATIC_FORCEINLINE void configure_led()
 {
   // function 5 = SIO 
   IO_BANK0->GPIO25_CTRL = 0x05;
   PADS_BANK0->GPIO25 = 0x34;
   // enable output
   SIO->GPIO_OE_SET = 0x01U << 25;
 }
 
 __STATIC_FORCEINLINE void turn_led_on()
 {
   SIO->GPIO_OUT_SET = 0x01U << 25;
 }
 
 __STATIC_FORCEINLINE void turn_led_off()
 {
   SIO->GPIO_OUT_CLR = 0x01U << 25;
 }
 
 __STATIC_FORCEINLINE void flip_led()
 {
   SIO->GPIO_OUT_XOR = 0x01U << 25;
 }

void cpu_init(void)
{
    cpu_clock_init();

    /* initialize the Cortex-M core */
    cortexm_init();

    configure_led();
    turn_led_on();

    _cpu_reset();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();
}
