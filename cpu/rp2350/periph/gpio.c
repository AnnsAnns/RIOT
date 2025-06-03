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
}

bool gpio_read(gpio_t pin) {}

void gpio_set(gpio_t pin) {}

void gpio_clear(gpio_t pin) {}

void gpio_toggle(gpio_t pin) {}

void gpio_write(gpio_t pin, bool value) {}
