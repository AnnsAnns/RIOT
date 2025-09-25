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

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const void* vector_cpu[CPU_IRQ_NUMOF];

void xh3irq_handler(void);
void xh3irq_enable_irq(uint32_t irq_no);
void xh3irq_disable_irq(uint32_t irq_no);
void xh3irq_force_irq(uint32_t irq_no);
uin32_t xh3irq_has_pending(void);

#ifdef __cplusplus
}
#endif

/** @} */
