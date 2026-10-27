/*
 * SPDX-FileCopyrightText: 2026 RIOT contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_bao1x
 * @{
 *
 * @file
 * @brief       CPU initialization for the Baochip-1x
 */

#include "cpu.h"
#include "periph/init.h"

void cpu_init(void)
{
    /* The boot ROM has already configured the PLLs, RRAM and SRAM.
     * Set up traps/interrupts and optional CPU features here. */
    riscv_init();

    /* Initialize the peripheral drivers (USB, GPIO, ...). */
    periph_init();
}

/** @} */
