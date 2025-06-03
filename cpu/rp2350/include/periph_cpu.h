#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "cpu.h"
#include "RP2350.h"

#include "uart_conf.h"
#include "clock_conf.h"
#include "helpers.h"

// Overwrite the default GPIO type to use uint32_t
#define HAVE_GPIO_T
typedef uint32_t gpio_t;

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

void gpio_reset(void) {
    reset_component(RESET_PADS_BANK0, RESET_PADS_BANK0);
    reset_component(RESET_IO_BANK0, RESET_IO_BANK0);
}

// Get it, Pin + Init, hahahaha
void pinit(void) {
    // Set LED (25) and Pin 15 so we can debug with them
    IO_BANK0->GPIO15_CTRL = GPIO_FUNC_SIO;
    IO_BANK0->GPIO25_CTRL = GPIO_FUNC_SIO;

    // Clear the ISO bits for GPIO15 and GPIO25
    // Otherwise the GPIOs will not work
    atomic_clear(&PADS_BANK0->GPIO15, PADS_BANK0_ISO_BITS);
    atomic_clear(&PADS_BANK0->GPIO15, PADS_BANK0_GPIO0_IE_BITS);
    atomic_clear(&PADS_BANK0->GPIO25, PADS_BANK0_ISO_BITS);
    
    // Set the GPIO function for GPIO15 and GPIO25
    // GPIO25 is used for the LED
    // GPIO15 is used for debugging via Oscilloscope
    SIO->GPIO_OE_SET = 1<<15 | 1<<25;
    SIO->GPIO_OUT = 1<<15 | 1<<25;
}

static inline volatile uint32_t* calculate_gpio_pad_register_addr(gpio_t pin) {
    // Each pin has a 4 byte register, so we can calculate the address
    // by adding 4 bytes for each pin, starting at the base address of PADS_BANK0
    // and adding 4 bytes to skip VOLTAGE_SELECT
    return (volatile uint32_t*)(PADS_BANK0_BASE + 4 * (pin + 1));
}

#ifdef __cplusplus
}
#endif

