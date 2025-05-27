/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include "cpu.h"

#include "RP2350.h"
#include "board.h"
#include "kernel_init.h"
#include "macros/units.h"
#include "periph/init.h"
#include "periph_cpu.h"
#include "stdio_base.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define MHZ(x) ((x) * 1000000u)
#define RESET_PLL_SYS 1 << 14
#define RESET_PADS_BANK0 1 << 9
#define RESET_UART0 1 << 26
#define RESET_UART1 1 << 27
#define RESET_IO_BANK0 1 << 6
#define XOSC_HZ MHZ(12)
#define REF_DIV 2
#define FBDIV 125
#define VCO_FREQ 750000000u
#define PD1 6
#define PD2 2
#define PLL_PWR_PD_BITS 0x00000001u
#define PLL_PWR_VCOPD_BITS 0x00000020u
#define PLL_CS_LOCK_BITS 0x80000000u
#define PLL_PRIM_POSTDIV1_LSB 16u
#define PLL_PRIM_POSTDIV2_LSB 12u
#define PLL_PWR_POSTDIVPD_BITS   0x00000008u
#define CLK_PERI_CTRL_ENABLE_BIT 1 << 11
#define XOSC_FREQ 12000000
#define POSTDIV1 6
#define POSTDIV2 2
#define CPUFREQ 125000000
#define FBDIV (CPUFREQ*POSTDIV1*POSTDIV2/XOSC_FREQ)

#define BAUDRATE 921600
#define IBRD ((8*CPUFREQ + BAUDRATE)/(2*BAUDRATE))/64
#define FBRD ((8*CPUFREQ + BAUDRATE)/(2*BAUDRATE))%64

/**
 * @name    Ranges for clock frequencies and clock settings
 * @{
 */
#define CLOCK_XOSC_MAX              MHZ(15) /**< Maximum crystal frequency */
#define CLOCK_XOSC_MIN              MHZ(5)  /**< Minimum crystal frequency */
#define CLOCK_XOSC              (XOSC_HZ) /**< Crystal frequency */
#define PLL_POSTDIV_MIN             (1U)    /**< Minimum value of the post PLL clock divers */
#define PLL_POSTDIV_MAX             (7U)    /**< Maximum value of the post PLL clock divers */
#define PLL_VCO_FEEDBACK_SCALE_MIN  (16U)   /**< Minimum value of the PLL VCO feedback scaler */
#define PLL_VCO_FEEDBACK_SCALE_MAX  (320U)  /**< Maximum value of the PLL VCO feedback scaler */
#define PLL_REF_DIV_MIN             (1U)    /**< Minimum value of the clock divider applied before
                                             *   feeding in the reference clock into the PLL */
#define PLL_REF_DIV_MAX             (1U)    /**< Minimum value of the clock divider applied before
                                             *   feeding in the reference clock into the PLL */
/** @} */
#define UART_UARTCR_UARTEN_BITS 1<<0
#define UART_UARTCR_RXE_BITS 1<<9
#define UART_UARTCR_TXE_BITS 1<<8
#define     PADS_BANK0_GPIO0_IE_BITS 1<<6

#if (PD1 < PLL_POSTDIV_MIN) || (PD1 > PLL_POSTDIV_MAX)
#error "Value for PLL_PRIM_POSTDIV1 out of range, check config"
#endif
#if (PD2 < PLL_POSTDIV_MIN) || (PD2 > PLL_POSTDIV_MAX)
#error "Value for PLL_PRIM_POSTDIV2 out of range, check config"
#endif
#if (PLL_VCO_FEEDBACK_SCALE_MIN < PLL_VCO_FEEDBACK_SCALE_MIN) || (PLL_VCO_FEEDBACK_SCALE_MAX > PLL_VCO_FEEDBACK_SCALE_MAX)
#error "Value for PLL_VCO_FEEDBACK_SCALE out of range, check config"
#endif
#if (PLL_REF_DIV_MIN < PLL_REF_DIV_MIN) || (PLL_REF_DIV_MAX > PLL_REF_DIV_MAX)
#error "Value for PLLxosc_sleep_REF_DIV out of range, check config"
#endif
#if (PLL_POSTDIV_MIN < PLL_POSTDIV_MIN) || (PLL_POSTDIV_MAX > PLL_POSTDIV_MAX)
#error "Value for PLL_POSTDIV out of range, check config"
#endif
#if (CLOCK_XOSC > CLOCK_XOSC_MAX) || (CLOCK_XOSC < CLOCK_XOSC_MIN)
#error "Value for CLOCK_XOSC out of range, check config"
#endif

#define PDIV ((PD1 << PLL_PRIM_POSTDIV1_LSB) | (PD2 << PLL_PRIM_POSTDIV2_LSB))
#define FBDIV ((VCO_FREQ / XOSC_HZ) / REF_DIV)

static void _cpu_reset(void) {
  LED0_OFF;

    uint32_t mask = RESET_PLL_SYS | RESET_PADS_BANK0 | RESET_IO_BANK0 | RESET_UART0 | RESET_UART1;


  // Reset based on 7.5.2
  RESETS->RESET = mask;
    RESETS->RESET = RESETS->RESET & ~mask;

  // Wait for the reset to complete
  while (!((RESETS->RESET_DONE & RESET_PLL_SYS) &&
           (RESETS->RESET_DONE & RESET_PADS_BANK0) &&
           (RESETS->RESET_DONE & RESET_IO_BANK0) &&
           (RESETS->RESET_DONE & RESET_UART0) &&
           (RESETS->RESET_DONE & RESET_UART1))) {
  }
}

/**
 * @brief Configure the clock system
 */
static void cpu_clock_init(void) { 
    // Enable the XOSC
    xosc_start(); 

    /**
     * Setup the PLL using the XOSC as the reference clock.
     */
    //PLL_SYS->CS = REF_DIV; // Set the reference divider
    PLL_SYS->FBDIV_INT = 125; // Set the feedback divider

    uint32_t power = PLL_PWR_PD_BITS | PLL_PWR_VCOPD_BITS | PLL_PWR_POSTDIVPD_BITS;

    /**
     * Set the post-dividers for the PLL output.
     */
    PLL_SYS->PRIM = PDIV;
    // Turn on PLL
    PLL_SYS->PWR = PLL_SYS->PWR & ~power;

    // sleep 20ms
    xosc_sleep(20);

    // // Wait for lock
    // while (!(PLL_SYS->CS & PLL_CS_LOCK_BITS)) {
    //     // Wait for the PLL to lock
    // }

    // AUXSRC = 0x0 7:5 && SRC == 0x0 0 
    // CLOCKS: CLK_SYS_CTRL CLK_PERI_CTRL_ENABLE_BIT
    CLOCKS->CLK_SYS_CTRL = 1;

    // This register contains one decoded bit for each of the clock sources enumerated in the CTRL SRC field.
    // The bit does not directly correlate with the value of the SRC field
    // For example 0x0 is the first bit while 0x1 is the second bit
    while (CLOCKS->CLK_SYS_SELECTED != 2) {}

    // src: CLOCKS: CLK_PERI_CTRL 
    // AUXSRC = 0x0 -> CLK_SYS Indirectly through lower line
    CLOCKS->CLK_PERI_CTRL = CLK_PERI_CTRL_ENABLE_BIT;
}

#define GPIO_FUNC_SIO 5
#define GPIO_FUNC_UART 2
#define PADS_BANK0_ISO_BITS 1<<8

void clock_reset(void) {
    // Reset the clock system
    reset_component(RESET_PLL_SYS, RESET_PLL_SYS);
}

void gpio_reset(void) {
    reset_component(RESET_PADS_BANK0, RESET_PADS_BANK0);
    reset_component(RESET_IO_BANK0, RESET_IO_BANK0);
}

// Get it, Pin + Init, hahahaha
void pinit(void) {
    // Set LED (25) and Pin 15 so we can debug with them
    IO_BANK0->GPIO15_CTRL = GPIO_FUNC_SIO;
    IO_BANK0->GPIO25_CTRL = GPIO_FUNC_SIO;

    // Clear the ISO bits for GPIO15 and GPIO25
    // Otherwise the GPIOs will not work
    atomic_bitmask_clear(&PADS_BANK0->GPIO15, PADS_BANK0_ISO_BITS);
    atomic_bitmask_clear(&PADS_BANK0->GPIO15, PADS_BANK0_GPIO0_IE_BITS);
    atomic_bitmask_clear(&PADS_BANK0->GPIO25, PADS_BANK0_ISO_BITS);
    
    // Set the GPIO function for GPIO15 and GPIO25
    // GPIO25 is used for the LED
    // GPIO15 is used for debugging via Oscilloscope
    SIO->GPIO_OE_SET = 1<<15 | 1<<25;
    SIO->GPIO_OUT = 1<<15 | 1<<25;
}

void uartinit(void) {
    // Set the UART pins to the correct function
    IO_BANK0->GPIO0_CTRL = GPIO_FUNC_UART;
    IO_BANK0->GPIO1_CTRL = GPIO_FUNC_UART;
    // Clear the ISO bits
    PADS_BANK0->GPIO0 = PADS_BANK0->GPIO0 & ~PADS_BANK0_ISO_BITS;
    PADS_BANK0->GPIO1 = PADS_BANK0->GPIO1 & ~PADS_BANK0_ISO_BITS;
    // Set IE bit for gpio1
    PADS_BANK0->GPIO1 = PADS_BANK0->GPIO1 | PADS_BANK0_GPIO0_IE_BITS;

    RESETS->RESET = RESETS->RESET & ~RESET_UART0;

    while (!(RESETS->RESET_DONE & RESET_UART0)) {
    }

    UART0->UARTIBRD = IBRD;
    UART0->UARTFBRD = FBRD;
    // 8bits, no parity, 1 stop bit
    UART0->UARTLCR_H = 0b11<<5;
    UART0->UARTCR = UART_UARTCR_RXE_BITS | UART_UARTCR_TXE_BITS | UART_UARTCR_UARTEN_BITS;

}

void cpu_init(void) {
  /* initialize the Cortex-M core */
  //cortexm_init();
  pinit();

  //_cpu_reset();

  /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
  early_init();

  cpu_clock_init();

  /* trigger static peripheral initialization */
  periph_init();
}
