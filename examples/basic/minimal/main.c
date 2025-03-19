#include <stdio.h>
#include "RP2350.h"

__STATIC_FORCEINLINE void configure_led()
{
  // function 5 = SIO 
  IO_BANK0->GPIO25_CTRL = 0x05;
  PADS_BANK0->GPIO25 = 0x34;
  // enable output
  SIO->GPIO_OE_SET = 0x01U << 25;
}

__STATIC_FORCEINLINE void turn_led_on()
{
  SIO->GPIO_OUT_SET = 0x01U << 25;
}

__STATIC_FORCEINLINE void turn_led_off()
{
  SIO->GPIO_OUT_CLR = 0x01U << 25;
}

__STATIC_FORCEINLINE void flip_led()
{
  SIO->GPIO_OUT_XOR = 0x01U << 25;
}


int main(void) {
    // This will cause our program to never return
    // but also means that we can be sure nothing else is running
    while (1) {
        for (int i = 0; i < 75000000; i++) __NOP();
        flip_led();
        printf("Hello, World!\n");
    }
    
    volatile int i = 1;
    
    while (i) {
    }

    return 0;
}
