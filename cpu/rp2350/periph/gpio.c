#include "periph/gpio.h"

#include <errno.h>

#include "board.h"
#include "irq.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define GPIO_PIN_NUMOF 30U


int gpio_init(gpio_t pin, gpio_mode_t mode) {
    // Check if we exceed the maximum number of GPIO pins
    assert(pin < GPIO_PIN_NUMOF);

    switch (mode) {
        case GPIO_OUT:
            volatile uint32_t *io_ctrl_reg = (uint32_t*)calculate_gpio_io_ctrl_register_addr(pin);
            volatile uint32_t *pads_ctrl_reg = (uint32_t*)calculate_gpio_pad_register_addr(pin);

            *io_ctrl_reg = FUNCTION_SELECT_SIO;

            // Clear the IE Bite to disable input
            atomic_clear(pads_ctrl_reg, PADS_BANK0_GPIO0_IE_BITS);

            // Clear the ISO bit to enable the GPIO
            atomic_clear(pads_ctrl_reg, PADS_BANK0_ISO_BITS);

            SIO->GPIO_OE_SET = 1 << pin; // Set the pin as output
            SIO->GPIO_OUT = 1 << pin; // Initialize the pin to HIGH

            break;
        default:
            // Unsupported mode
            return -ENOTSUP;
    }
}

bool gpio_read(gpio_t pin) {}

void gpio_set(gpio_t pin) {
    SIO->GPIO_OUT_SET = 1 << pin; // Set the pin to HIGH
}

void gpio_clear(gpio_t pin) {
    SIO->GPIO_OUT_CLR = 1 << pin; // Set the pin to LOW
}

void gpio_toggle(gpio_t pin) {
    SIO->GPIO_OUT_XOR = 1 << pin; // Toggle the pin state (XOR)
}

void gpio_write(gpio_t pin, bool value) {
    if (value) {
        gpio_set(pin); // Set the pin to HIGH
    } else {
        gpio_clear(pin); // Set the pin to LOW
    }
}
