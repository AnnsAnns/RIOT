#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#define HAVE_GPIO_T

#include <stdbool.h>
#include <stdint.h>

#include "cpu.h"
#include "RP2350.h"

#include "uart_conf.h"
#include "clock_conf.h"
#include "helpers.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
typedef uint32_t gpio_t;

#ifdef __cplusplus
}
#endif

#endif
