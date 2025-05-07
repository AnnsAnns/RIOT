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
#include "kernel_init.h"
#include "macros/units.h"
#include "periph/init.h"
#include "stdio_base.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _cpu_reset(void) {
//   /* power the reference clock from its default source: the ROSC */
//   clock_ref_configure_source(MHZ(12), MHZ(12),
//                              CLOCKS_CLK_REF_CTRL_SRC_rosc_clksrc_ph);
//   /* power the system clock from its default source: the reference clock */
//   clock_sys_configure_source(MHZ(12), MHZ(12), CLOCKS_CLK_SYS_CTRL_SRC_clk_ref);
//   /* start XOSC, typically running at 12 MHz */
//   xosc_start(CLOCK_XOSC);
//   /* configure reference clock to run from XOSC (typically 12 MHz) */
//   clock_ref_configure_source(CLOCK_XOSC, CLOCK_XOSC,
//                              CLOCKS_CLK_REF_CTRL_SRC_xosc_clksrc);
//     /* configure the peripheral clock to run from the system clock */
    clock_periph_configure(CLOCK_PERIPH_SOURCE);
}

/**
 * @brief Configure the clock system
 */
static void cpu_clock_init(void) {}

void cpu_init(void) {
  cpu_clock_init();

  /* initialize the Cortex-M core */
  cortexm_init();

  _cpu_reset();

  /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
  early_init();

  /* trigger static peripheral initialization */
  periph_init();
}
