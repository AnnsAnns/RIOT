#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

int main(void) {
  while (true) {
    xosc_sleep(100);
    SIO->GPIO_OUT_XOR = 1 << 15;
    LED0_TOGGLE;
  }
  return 0;
}
