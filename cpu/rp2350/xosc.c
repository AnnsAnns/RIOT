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
 * @brief       Implementation of the crystal oscillator (XOSC)
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <assert.h>

#include "macros/units.h"
#include "RP2350.h"
#include "io_reg.h"
#include "periph_cpu.h"

// Based on datasheet 8.2.4 (1ms wait time)
#define STARTUP_DELAY 47

void xosc_start(uint32_t f_ref)
{
    // Set the FREQ_RANGE
    XOSC->CTRL = f_ref;
    // Set the startup delay (default 1ms)
    XOSC->STARTUP  = STARTUP_DELAY;
    // set enable bit
    io_reg_atomic_set(&XOSC->CTRL, XOSC_CTRL_ENABLE_VALUE_ENABLE << XOSC_CTRL_ENABLE_LSB);

    while (!(XOSC->STATUS & XOSC_STATUS_STABLE_BITS)) {
        // Wait for the crystal to stabilize
    }
}

void xosc_stop(void)
{
    // @TODO
}
