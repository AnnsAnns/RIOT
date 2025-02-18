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
#include "io_reg.h"
#include "stdio_base.h"

#define ENABLE_DEBUG        0
#include "debug.h"

static void _cpu_reset(void)
{
}

void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

    _cpu_reset();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();
}
