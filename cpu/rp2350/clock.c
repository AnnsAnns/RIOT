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
 * @brief       Implementation of the CPU clock configuration
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "RP2350.h"
#include "io_reg.h"
#include "periph_cpu.h"

static void _clk_sys_set_source(CLOCKS_CLK_SYS_CTRL_SRC_Enum source)
{
    io_reg_write_dont_corrupt(&CLOCKS->CLK_SYS_CTRL, source << CLOCKS_CLK_SYS_CTRL_SRC_Pos,
                              CLOCKS_CLK_SYS_CTRL_SRC_Msk);
}

static void _clk_ref_set_source(CLOCKS_CLK_REF_CTRL_SRC_Enum source)
{
    io_reg_write_dont_corrupt(&CLOCKS->CLK_REF_CTRL, source << CLOCKS_CLK_REF_CTRL_SRC_Pos,
                              CLOCKS_CLK_REF_CTRL_SRC_Msk);
}

void clock_sys_configure_source(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_SYS_CTRL_SRC_Enum source)
{
    assert(f_out <= f_in);
    assert(source != CLOCKS_CLK_SYS_CTRL_SRC_clksrc_clk_sys_aux);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_SYS_DIV_INT_Pos) / f_out;
    /* switch the glitchless mux to clk_ref */
    _clk_sys_set_source(source);
    /* apply divider */
    CLOCKS->CLK_SYS_DIV = div;
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_SYS_SELECTED & (1U << source))) { }
}

void clock_ref_configure_source(uint32_t f_in, uint32_t f_out, CLOCKS_CLK_REF_CTRL_SRC_Enum source)
{
    assert(f_out <= f_in);
    assert(source != CLOCKS_CLK_REF_CTRL_SRC_clksrc_clk_ref_aux);
    uint32_t div = (((uint64_t)f_in) << CLOCKS_CLK_REF_DIV_INT_Pos) / f_out;
    /* switch the glitchless mux to clock source */
    _clk_ref_set_source(source);
    /* apply divider */
    CLOCKS->CLK_REF_DIV = div & CLOCKS_CLK_REF_DIV_INT_Msk;
    /* poll SELECTED until the switch is completed */
    while (!(CLOCKS->CLK_REF_SELECTED & (1U << source))) { }
}

void clock_periph_configure(CLOCKS_CLK_PERI_CTRL_AUXSRC_Enum aux)
{
    io_reg_atomic_clear(&CLOCKS->CLK_PERI_CTRL, (1u << CLOCKS_CLK_PERI_CTRL_ENABLE_Pos));
    io_reg_write_dont_corrupt(&CLOCKS->CLK_PERI_CTRL, aux << CLOCKS_CLK_PERI_CTRL_AUXSRC_Pos,
                              CLOCKS_CLK_PERI_CTRL_AUXSRC_Msk);
    io_reg_atomic_set(&CLOCKS->CLK_PERI_CTRL, (1u << CLOCKS_CLK_PERI_CTRL_ENABLE_Pos));
}
