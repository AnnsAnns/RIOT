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
 * @brief       Baochip-1x external interrupt dispatch
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
 * @brief   Timer0 interrupt line (used for the USB OUT doorbell kick)
 */
#define BAO1X_USB_TIMER_IRQ (30U)

/**
 * @brief   TickTimer interrupt line
 */
#define BAO1X_TICKTIMER_IRQ (20U)

/**
 * @brief   USB device controller interrupt line
 */
#define BAO1X_USB_IRQ       (1U)

/**
 * @brief   Enable an external interrupt line
 *
 * @param[in] irq   interrupt line to enable
 */
void bao1x_irq_enable(unsigned irq);

#ifdef __cplusplus
}
#endif

/** @} */
