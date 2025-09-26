/*
 * Copyright (C) 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_fe310
 * @{
 *
 * @file
 * @brief           CPU specific configuration options
 *
 * @author          Ken Rabold
 */

#include "cpu_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Default IRQ priority for the CPU,
 * we don't have different levels at the moment */
#define CPU_DEFAULT_IRQ_PRIO 1u
/** The RP2350 has 52 IRQs, see vectors.c for more details */
#define CPU_IRQ_NUMOF 52u
/** The Hazard3 core supports 16 PMP entries */
#define NUM_PMP_ENTRIES 16u
/** This tells rp2350_common that we are using the RISC-V core */
#define RP2350_USE_RISCV 1

#ifdef __cplusplus
}
#endif

/** @} */
