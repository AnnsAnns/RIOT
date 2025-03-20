#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdbool.h>
#include <stdint.h>

#include "cpu.h"
#include "RP2350.h"
#include "io_reg.h"
#include "pll_conf.h"
#include "uart_conf.h"

#define PROVIDES_PM_SET_LOWEST

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/**
 * @brief   Convert (port, pin) tuple to a @ref gpio_t value
 *
 * @note    The RPX0XX MCU family only has a single GPIO port. Still the API requires a
 *          port parameter, which is currently ignored.
 */
#define GPIO_PIN(port, pin)     ((((port) & 0)) | (pin))

/**
 * @brief   Waits until hardware components have been reset
 *
 * @param   components bitmask of components that must have reset,
 *          @see RESETS_RESET_MASK
 */
static inline void periph_reset_done(uint32_t components)
{
    io_reg_atomic_clear(&RESETS->RESET, components);
    while ((~RESETS->RESET_DONE) & components) { }
}

/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;

/**
 * @brief   Configuration details for an UART interface needed by the RPX0XX peripheral
 */
typedef struct {
    UART0_Type *dev;    /**< Base address of the I/O registers of the device */
    gpio_t rx_pin;      /**< GPIO pin to use for RX */
    gpio_t tx_pin;      /**< GPIO pin to use for TX */
    IRQn_Type irqn;     /**< IRQ number of the UART interface */
} uart_conf_t;

/**
 * @brief   Memory layout of GPIO control register in pads bank 0
 */
typedef struct {
    uint32_t slew_rate_fast         : 1;    /**< set slew rate control to fast */
    uint32_t schmitt_trig_enable    : 1;    /**< enable Schmitt trigger */
    uint32_t pull_down_enable       : 1;    /**< enable pull down resistor */
    uint32_t pull_up_enable         : 1;    /**< enable pull up resistor */
    uint32_t drive_strength         : 2;    /**< GPIO driver strength */
    uint32_t input_enable           : 1;    /**< enable as input */
    uint32_t output_disable         : 1;    /**< disable output, overwrite output enable from
                                             *   peripherals */
    uint32_t                        : 24;   /**< 24 bits reserved for future use */
} gpio_pad_ctrl_t;
/** @} */

/**
 * @brief   Possible drive strength values for @ref gpio_pad_ctrl_t::drive_strength
 */
enum {
    DRIVE_STRENGTH_2MA,         /**< set driver strength to 2 mA */
    DRIVE_STRENGTH_4MA,         /**< set driver strength to 4 mA */
    DRIVE_STRENGTH_8MA,         /**< set driver strength to 8 mA */
    DRIVE_STRENGTH_12MA,        /**< set driver strength to 12 mA */
    DRIVE_STRENGTH_NUMOF        /**< number of different drive strength options */
};

#endif
