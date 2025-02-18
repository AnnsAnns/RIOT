#ifndef CPU_CONF_H
#define CPU_CONF_H

#define __CM0PLUS_REV 1    /*!< Core revision r0p1 */
#define __MPU_PRESENT 0    /*!< MPU present or not */
#define __NVIC_PRIO_BITS 2 /*!< Number of bits used for Priority Levels */
#define __VTOR_PRESENT 1   /*!< VTOR present or not */
#define __Vendor_SysTickConfig \
  0 /*!< Set to 1 if different SysTick Config is used */

#define __CHECK_DEVICE_DEFINES

#define CPU_DEFAULT_IRQ_PRIO (1U)
#define CPU_IRQ_NUMOF (26U)

typedef enum {
  /* =  ARM Cortex-M0 Specific Interrupt Numbers  = */
  Reset_IRQn =
      -15, /*!< -15  Reset Vector, invoked on Power up and warm reset */
  NonMaskableInt_IRQn =
      -14, /*!< -14  Non maskable Interrupt, cannot be stopped or preempted */
  HardFault_IRQn = -13,  /*!< -13  Hard Fault, all classes of Fault */
  SVCall_IRQn = -5,      /*!< -5 System Service Call via SVC instruction */
  PendSV_IRQn = -2,      /*!< -2 Pendable request for system service */
  SysTick_IRQn = -1,     /*!< -1 System Tick Timer */
                         /* =  rp2350 Specific Interrupt Numbers  = */
  TIMER0_IRQ_0 = 0,      /*!< 0  TIMER0_IRQ_0 */
  TIMER0_IRQ_1 = 1,      /*!< 1  TIMER0_IRQ_1 */
  TIMER0_IRQ_2 = 2,      /*!< 2  TIMER0_IRQ_2 */
  TIMER0_IRQ_3 = 3,      /*!< 3  TIMER0_IRQ_3 */
  TIMER1_IRQ_0 = 4,      /*!< 4  TIMER1_IRQ_0 */
  TIMER1_IRQ_1 = 5,      /*!< 5  TIMER1_IRQ_1 */
  TIMER1_IRQ_2 = 6,      /*!< 6  TIMER1_IRQ_2 */
  TIMER1_IRQ_3 = 7,      /*!< 7Vectors   TIMER1_IRQ_3 */
  PWM_IRQ_WRAP_0 = 8,    /*!< 8  PWM_IRQ_WRAP_0 */
  PWM_IRQ_WRAP_1 = 9,    /*!< 9  PWM_IRQ_WRAP_1 */
  DMA_IRQ_0 = 10,        /*!< 10 DMA_IRQ_0 */
  DMA_IRQ_1 = 11,        /*!< 11 DMA_IRQ_1 */
  DMA_IRQ_2 = 12,        /*!< 12 DMA_IRQ_2 */
  DMA_IRQ_3 = 13,        /*!< 13 DMA_IRQ_3 */
  USBCTRL_IRQ = 14,      /*!< 14 USBCTRL_IRQ */
  PIO0_IRQ_0 = 15,       /*!< 15 PIO0_IRQ_0 */
  PIO0_IRQ_1 = 16,       /*!< 16 PIO0_IRQ_1 */
  PIO1_IRQ_0 = 17,       /*!< 17 PIO1_IRQ_0 */
  PIO1_IRQ_1 = 18,       /*!< 18 PIO1_IRQ_1 */
  PIO2_IRQ_0 = 19,       /*!< 19 PIO2_IRQ_0 */
  PIO2_IRQ_1 = 20,       /*!< 20 PIO2_IRQ_1 */
  IO_IRQ_BANK0 = 21,     /*!< 21 IO_IRQ_BANK0 */
  IO_IRQ_BANK0_NS = 22,  /*!< 22 IO_IRQ_BANK0_NS */
  IO_IRQ_QSPI = 23,      /*!< 23 IO_IRQ_QSPI */
  IO_IRQ_QSPI_NS = 24,   /*!< 24 IO_IRQ_QSPI_NS */
  SIO_IRQ_FIFO = 25,     /*!< 25 SIO_IRQ_FIFO */
  SIO_IRQ_BELL = 26,     /*!< 26 SIO_IRQ_BELL */
  SIO_IRQ_FIFO_NS = 27,  /*!< 27 SIO_IRQ_FIFO_NS */
  SIO_IRQ_BELL_NS = 28,  /*!< 28 SIO_IRQ_BELL_NS */
  SIO_IRQ_MTIMECMP = 29, /*!< 29 SIO_IRQ_MTIMECMP */
  CLOCKS_IRQ = 30,       /*!< 30 CLOCKS_IRQ */
  SPI0_IRQ = 31,         /*!< 31 SPI0_IRQ */
  SPI1_IRQ = 32,         /*!< 32 SPI1_IRQ */
  UART0_IRQ = 33,        /*!< 33 UART0_IRQ */
  UART1_IRQ = 34,        /*!< 34 UART1_IRQ */
  ADC_IRQ_FIFO = 35,     /*!< 35 ADC_IRQ_FIFO */
  I2C0_IRQ = 36,         /*!< 36 I2C0_IRQ */
  I2C1_IRQ = 37,         /*!< 37 I2C1_IRQ */
  OTP_IRQ = 38,          /*!< 38 OTP_IRQ */
  TRNG_IRQ = 39,         /*!< 39 TRNG_IRQ */
  PROC0_IRQ_CTI = 40,    /*!< 40 PROC0_IRQ_CTI */
  PROC1_IRQ_CTI = 41,    /*!< 41 PROC1_IRQ_CTI */
  PLL_SYS_IRQ = 42,      /*!< 42 PLL_SYS_IRQ */
  PLL_USB_IRQ = 43,      /*!< 43 PLL_USB_IRQ */
  POWMAN_IRQ_POW = 44,   /*!< 44 POWMAN_IRQ_POW */
  POWMAN_IRQ_TIMER = 45, /*!< 45 POWMAN_IRQ_TIMER */
  SPAREIRQ_IRQ_0 = 46,   /*!< 46 SPAREIRQ_IRQ_0 */
  SPAREIRQ_IRQ_1 = 47,   /*!< 47 SPAREIRQ_IRQ_1 */
  SPAREIRQ_IRQ_2 = 48,   /*!< 48 SPAREIRQ_IRQ_2 */
  SPAREIRQ_IRQ_3 = 49,   /*!< 49 SPAREIRQ_IRQ_3 */
  SPAREIRQ_IRQ_4 = 50,   /*!< 50 SPAREIRQ_IRQ_4 */
  SPAREIRQ_IRQ_5 = 51    /*!< 51 SPAREIRQ_IRQ_5 */
} IRQn_Type;

#include "cpu_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
