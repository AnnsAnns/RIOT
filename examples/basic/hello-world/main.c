/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include "multicore.h"
#include "periph_cpu.h"
#include <stdio.h>
#include "periph/gpio.h"

#define PIN_14 14u
#define PIN_15 15u

void* core1_main(void *arg) {
    (void)arg;
    gpio_init(PIN_14, GPIO_OUT);
    while (1) {
        gpio_set(PIN_14);
        gpio_clear(PIN_14);
    }
    return NULL;
}

int main(void) {
    core1_init(core1_main, NULL);
    gpio_init(PIN_15, GPIO_OUT);
    while (1) {
        gpio_set(PIN_15);
        gpio_clear(PIN_15);
    }
    return 0;
}
