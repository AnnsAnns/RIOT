/*
 * SPDX-FileCopyrightText: 2026 RIOT contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_bao1x
 * @{
 *
 * @file
 * @brief       Baochip-1x interrupt and Timer0 helpers
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   VexRiscv custom interrupt CSRs
 * @{
 */
#define BAO1X_CSR_MIM       (0xbc0) /**< machine interrupt mask */
#define BAO1X_CSR_MIP_VEX   (0xfc0) /**< pending external interrupt lines */
/** @} */

/**
 * @brief   Timer0 interrupt line
 */
#define BAO1X_TIMER0_IRQ    (30U)

/**
 * @brief   USB device controller interrupt line
 */
#define BAO1X_USB_IRQ       (1U)

/**
 * @brief   Timer0 base address
 */
#define BAO1X_TIMER0_BASE   (0xe001c000UL)

/**
 * @brief   Register a Timer0 callback
 */
void bao1x_timer0_set_callback(void (*cb)(void *), void *arg);

/**
 * @brief   Start Timer0 as a periodic down-counter
 *
 * @param[in] load  reload value in ACLK cycles
 */
void bao1x_timer0_start(uint32_t load);

/**
 * @brief   Stop Timer0
 */
void bao1x_timer0_stop(void);

#ifdef __cplusplus
}
#endif

/** @} */
