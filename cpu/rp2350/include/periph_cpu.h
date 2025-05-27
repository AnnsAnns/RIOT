#pragma once

#define HAVE_GPIO_T

#include <stdbool.h>
#include <stdint.h>

#include "cpu.h"
#include "RP2350.h"

#include "uart_conf.h"
#include "clock_conf.h"
#include "helpers.h"

#define RESET_PLL_SYS 1 << 14
#define RESET_PADS_BANK0 1 << 9
#define RESET_UART0 1 << 26
#define RESET_UART1 1 << 27
#define RESET_IO_BANK0 1 << 6

#define PADS_BANK0_GPIO0_IE_BITS 1<<6
#define GPIO_FUNC_SIO 5
#define GPIO_FUNC_UART 2
#define PADS_BANK0_ISO_BITS 1<<8

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

