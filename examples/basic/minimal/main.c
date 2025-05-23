#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

#define UART_UARTFR_RXFF_BITS 1<<6
#define UART_UARTFR_TXFE_BITS 1<<7

int main(void) {
    while (true) {
      xosc_sleep(100);
      SIO->GPIO_OUT_XOR = 1 << 15;
      LED0_TOGGLE;
    }
  return 0;
}
