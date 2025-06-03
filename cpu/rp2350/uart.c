#include "periph_cpu.h"
#include "uart_conf.h"
void uartinit(void) {
  // Set the UART pins to the correct function
  IO_BANK0->GPIO0_CTRL = FUNCTION_SELECT_UART;
  IO_BANK0->GPIO1_CTRL = FUNCTION_SELECT_UART;
  // Clear the ISO bits
  atomic_clear(&PADS_BANK0->GPIO0, PADS_BANK0_ISO_BITS);
  atomic_clear(&PADS_BANK0->GPIO1, PADS_BANK0_ISO_BITS);
  // Set IE bit for gpio1
  PADS_BANK0->GPIO1 = PADS_BANK0->GPIO1 | PADS_BANK0_GPIO0_IE_BITS;

  // We reset UART0 here, so we can be sure it is in a known state
  reset_component(RESET_UART0, RESET_UART0);

  UART0->UARTIBRD = IBRD;
  UART0->UARTFBRD = FBRD;
  // 8bits, no parity, 1 stop bit
  UART0->UARTLCR_H = 0b11 << 5;
  UART0->UARTCR =
      UART_UARTCR_RXE_BITS | UART_UARTCR_TXE_BITS | UART_UARTCR_UARTEN_BITS;
}

void uart_send_char(char c) {
  while (!(UART0->UARTFR & UART_UARTFR_RXFF_BITS));
  UART0->UARTDR = c;
  while (!(UART0->UARTFR & UART_UARTFR_TXFE_BITS));
}

void uart_send_string(const char *str) {
  while (*str) {
    uart_send_char(*str++);
  }
}
