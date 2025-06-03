#include "cpu.h"

#include "RP2350.h"
#include "board.h"
#include "kernel_init.h"
#include "macros/units.h"
#include "periph/init.h"
#include "periph_cpu.h"
#include "stdio_base.h"

void cpu_init(void) {
  /* initialize the Cortex-M core */
  //cortexm_init();
  gpio_reset();
  pinit(); // Init LED + Oscilloscope Pin

  //_cpu_reset();

  /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
  early_init();

  clock_reset();
  cpu_clock_init();

  uartinit();

  /* trigger static peripheral initialization */
  periph_init();
}
