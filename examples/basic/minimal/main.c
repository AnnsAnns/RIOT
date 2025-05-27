#include <stdio.h>

#include "board.h"
#include "periph_conf.h"
#include <string.h>

#define UART_UARTFR_RXFF_BITS 1<<6
#define UART_UARTFR_TXFE_BITS 1<<7

int main(void) {
    // while (true) {
    //   xosc_sleep(100);
    //   SIO->GPIO_OUT_XOR = 1 << 15;
    //   LED0_TOGGLE;
    // }
    xosc_sleep(3000); // Sleep for 3 secs to connect
    uint32_t i = 0;
    while (true) {
        uart_send_string("Hello, RP2350!\n");
        xosc_sleep(1000); // Sleep for 1 sec
    }
  return 0;
}
