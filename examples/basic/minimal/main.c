#include <stdio.h>

#include "board.h"
#include "periph_conf.h"
#include <string.h>
#include "periph/gpio.h"

#define UART_UARTFR_RXFF_BITS 1<<6
#define UART_UARTFR_TXFE_BITS 1<<7

int main(void) {
    xosc_sleep(1000); // Sleep for 3 secs to connect

    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(OSC_DEBUG_PIN, GPIO_OUT);

    uint32_t i = 0;
    while (true) {
        gpio_toggle(LED0_PIN); // Toggle the LED
        uart_send_string("Hello, RP2350!\n");
        gpio_toggle(OSC_DEBUG_PIN); // Toggle the debug pin
        xosc_sleep(1000); // Sleep for 1 sec
    }
  return 0;
}
