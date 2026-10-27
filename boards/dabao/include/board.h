/*
 * SPDX-FileCopyrightText: 2026 RIOT contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_dabao
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Baochip-1x Dabao
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name USB SE0 / disconnect switch
 * @{
 */
#define BOARD_USB_SE0_PORT      (2)     /**< IOX port C */
#define BOARD_USB_SE0_PIN       (13)    /**< PC13 */
/** @} */

/**
 * @name Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK             (350000000UL)   /**< CPU clock */
#define CONFIG_ZTIMER_USEC_BASE_FREQ (1000UL)       /**< TickTimer runs at 1 kHz */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
