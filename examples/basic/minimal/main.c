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
    char *msg = "Hello, RP2350!\n";
    uint32_t i = 0;
    while (true) {
        // Wait for RX FIFO to be not full
        while(!(UART0->UARTFR & UART_UARTFR_RXFF_BITS));
        UART0->UARTDR = msg[i++];
        while(!(UART0->UARTFR & UART_UARTFR_TXFE_BITS));
        SIO->GPIO_OUT_XOR = 1 << 15;

        if (i >= strlen(msg)) {
            i = 0; // Reset index to loop through the message again
        }
    }
  return 0;
}
