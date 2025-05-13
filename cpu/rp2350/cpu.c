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

#include "RP2350.h"
#include "board.h"
#include "kernel_init.h"
#include "macros/units.h"
#include "periph/init.h"
#include "periph_cpu.h"
#include "stdio_base.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define RESET_PLL_SYS 1 << 14
#define RESET_PADS_BANK0 1 << 9
#define RESET_IO_BANK0 1 << 6
#define XOSC_HZ 12000000u
#define REF_DIV 2
#define FBDIV 125
#define VCO_FREQ 750000000u
#define PD1 6
#define PD2 1
#define PLL_PWR_PD_BITS 0x00000001u
#define PLL_PWR_VCOPD_BITS 0x00000020u
#define PLL_CS_LOCK_BITS 0x80000000u
#define PLL_PRIM_POSTDIV1_LSB 16u
#define PLL_PRIM_POSTDIV2_LSB 12u
#define PLL_PWR_POSTDIVPD_BITS   0x00000008u
#define CLK_PERI_CTRL_ENABLE_BIT 1 << 11

static void _cpu_reset(void) {
  LED0_OFF;

    uint32_t mask = RESET_PLL_SYS | RESET_PADS_BANK0 | RESET_IO_BANK0;


  // Reset based on 7.5.2
  RESETS->RESET = mask;
    RESETS->RESET = RESETS->RESET & ~mask;

  // Wait for the reset to complete
  while (!((RESETS->RESET_DONE & RESET_PLL_SYS) &&
           (RESETS->RESET_DONE & RESET_PADS_BANK0) &&
           (RESETS->RESET_DONE & RESET_IO_BANK0))) {
  }
}

/**
 * @brief Configure the clock system
 */
static void cpu_clock_init(void) { 
    xosc_start(); 

    uint32_t ref_freq = XOSC_HZ / REF_DIV;
    uint32_t fbdiv = VCO_FREQ / ref_freq;
    PLL_SYS->CS = REF_DIV; // Set the reference divider
    PLL_SYS->FBDIV_INT = fbdiv; // Set the feedback divider

    uint32_t power = PLL_PWR_PD_BITS | PLL_PWR_VCOPD_BITS;

    // Turn on PLL
    PLL_SYS->PWR = PLL_SYS->PWR & ~power;

    // Wait for lock
    while (!(PLL_SYS->CS & PLL_CS_LOCK_BITS)) {
        // Wait for the PLL to lock
    }

    uint32_t pdiv = (PD1 << PLL_PRIM_POSTDIV1_LSB) | (PD2 << PLL_PRIM_POSTDIV2_LSB);

    PLL_SYS->PRIM = pdiv;

    PLL_SYS->PWR = PLL_SYS->PWR & ~(1 << PLL_PWR_POSTDIVPD_BITS);

    // AUXSRC = 0x0 7:5 && SRC == 0x0 0 
    // CLOCKS: CLK_SYS_CTRL CLK_PERI_CTRL_ENABLE_BIT
    CLOCKS->CLK_SYS_CTRL = 1;

    // This register contains one decoded bit for each of the clock sources enumerated in the CTRL SRC field.
    while (!(CLOCKS->CLK_SYS_SELECTED == 1)) {}

    // src: CLOCKS: CLK_PERI_CTRL 
    CLOCKS->CLK_PERI_CTRL = CLK_PERI_CTRL_ENABLE_BIT;
}

void cpu_init(void) {
  /* initialize the Cortex-M core */
  cortexm_init();

  _cpu_reset();

  /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
  early_init();

  cpu_clock_init();

  /* trigger static peripheral initialization */
  periph_init();
}
