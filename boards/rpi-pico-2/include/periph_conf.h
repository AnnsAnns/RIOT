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
 
 #define NUM_UART ((unsigned int) 2)
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* PERIPH_CONF_H */
 /** @} */
 