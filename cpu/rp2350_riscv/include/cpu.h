/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_gd32v
 * @{
 *
 * @file
 * @brief       CPU specific definitions
 */

 /** Extra thread stack size if `printf` is used */
 #define THREAD_EXTRA_STACKSIZE_PRINTF   (1536)

 #ifndef THREAD_STACKSIZE_DEFAULT
 /** Default thread stack size */
 #define THREAD_STACKSIZE_DEFAULT      (2048)
 #endif

 #ifndef THREAD_STACKSIZE_IDLE
 /** Stack size for the idle thread */
 #define THREAD_STACKSIZE_IDLE         (2048)
 #endif

/** riscv_common uses inlined irq */
#define IRQ_API_INLINED

#include <stdbool.h>
#include <stdint.h>

/** Overwrite the default GPIO type to use uint32_t */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;

/* Im currently copying the original rp2040 def but this causes the other port to not be addressable (I think)*/
#define GPIO_PIN(port, pin)     (((port) & 0) | (pin))

/* This is a define used throughout the pico sdk */
#define _u(x) ((uint32_t)(x))

#include "periph_cpu.h"
#include "periph/gpio.h"

#include "irq_arch.h"
#include "clic.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
