 #ifndef PERIPH_CONF_H
 #define PERIPH_CONF_H
 
 #include <stdint.h>
 
 #include "periph_cpu.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 #define CLK_XTAL_FREQ (12000000) // 12MHz.
 
 /*
  * 0 = Do not use external crystal oscillator for clk_ref.
  * 1 = Use external crystal oscillator for clk_ref.
  */
 #define CLK_REF_XTAL (1)

 static const uart_conf_t uart_config[] = {
    {
        .dev = UART0,
        .rx_pin = GPIO_PIN(0, 1),
        .tx_pin = GPIO_PIN(0, 0),
        .irqn = UART0_IRQ_IRQn
    },
    {
        .dev = UART1,
        .rx_pin = GPIO_PIN(0, 9),
        .tx_pin = GPIO_PIN(0, 8),
        .irqn = UART1_IRQ_IRQn
    }
};

#define UART_0_ISR      (isr_uart0)
#define UART_1_ISR      (isr_uart1)

#define UART_NUMOF      ARRAY_SIZE(uart_config)
 
 #define NUM_UART ((unsigned int) 2)
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* PERIPH_CONF_H */
 /** @} */
 