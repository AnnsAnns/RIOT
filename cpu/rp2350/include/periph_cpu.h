#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#define HAVE_GPIO_T

#include <stdbool.h>
#include <stdint.h>

#include "cpu.h"
#include "RP2350.h"

#include "io_reg.h"
#include "pll_conf.h"
#include "uart_conf.h"
#include "timer_conf.h"
#include "clock_conf.h"

#define PROVIDES_PM_SET_LOWEST

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/**
 * @brief   Convert (port, pin) tuple to a @ref gpio_t value
 */
#define GPIO_PIN(port, pin)     ((((port) & port)) | (pin))

/**
 * @brief   Reset hardware components
 *
 * @param   components bitmask of components to be reset,
 *          @see RESETS_RESET_MASK
 */
static inline void periph_reset(uint32_t components)
{
    io_reg_atomic_set(&RESETS->RESET, components);
}


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
 * As seen in https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf#tab-registerlist_pads_bank0
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
    uint32_t pad_isolation_control :  1;    /**< isolate pad from chip */
    uint32_t                        : 23;   /**< 24 bits reserved for future use */
} gpio_pad_ctrl_t;
/** @} */

/**
 * @brief   Possible drive strength values for @ref gpio_pad_ctrl_t::drive_strengthio_reg_atomic_set
 */
enum {
    DRIVE_STRENGTH_2MA,         /**< set driver strength to 2 mA */
    DRIVE_STRENGTH_4MA,         /**< set driver strength to 4 mA */
    DRIVE_STRENGTH_8MA,         /**< set driver strength to 8 mA */
    DRIVE_STRENGTH_12MA,        /**< set driver strength to 12 mA */
    DRIVE_STRENGTH_NUMOF        /**< number of different drive strength options */
};

/**
 * @brief   Memory layout of GPIO control register in IO bank 0
 * https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf#tab-registerlist_io_bank0
 */
typedef struct {
    uint32_t function_select        : 5;    /**< select GPIO function (4:0) */
    uint32_t                        : 7;    /**< 7 bits reserved (11:5) */
    uint32_t output_override        : 2;    /**< output value override (13:12) */
    uint32_t output_enable_override : 2;    /**< output enable override (15:14)*/
    uint32_t input_override         : 2;    /**< input value override  (17:16)*/
    uint32_t                        : 10;   /**< 10 bits reserved for future use (27:18) */
    uint32_t irq_override           : 2;    /**< interrupt override (29:28)*/
    uint32_t                        : 2;    /**< 2 bits reserved for future use (31:30) */
} gpio_io_ctrl_t;
/** @} */

typedef enum {
    GPIO_OUTOVER_NORMAL   = 0,    /**< normal operation */
    GPIO_OUTOVER_INVERT = 1,    /**< inverted output */
    GPIO_OUTOVER_LOW    = 2,    /**< low output */
    GPIO_OUTOVER_HIGH   = 3,    /**< high output */
} gpio_over_options_t;

// Also within the IO_BANK_0 table (not on rp2040 for some reason though? Weird choice)
typedef enum {
    FUNCTION_SELECT_JTAG_TCK                 = 0x00,  /**< connect pin to JTAG TCK */
    FUNCTION_SELECT_SPI0_RX                  = 0x01,  /**< connect pin to SPI0 RX */
    FUNCTION_SELECT_UART0_TX                 = 0x02,  /**< connect pin to UART0 TX */
    FUNCTION_SELECT_I2C0_SDA                 = 0x03,  /**< connect pin to I2C0 SDA */
    FUNCTION_SELECT_PWM_A_0                  = 0x04,  /**< connect pin to PWM_A_0 */
    FUNCTION_SELECT_SIO_0                    = 0x05,  /**< use pin as vanilla GPIO (SIO) */
    FUNCTION_SELECT_PIO0_0                   = 0x06,  /**< connect pin to the first PIO peripheral */
    FUNCTION_SELECT_PIO1_0                   = 0x07,  /**< connect pin to the second PIO peripheral */
    FUNCTION_SELECT_PIO2_0                   = 0x08,  /**< connect pin to the third PIO peripheral */
    FUNCTION_SELECT_XIP_SS_N_1               = 0x09,  /**< connect pin to XIP SS_N_1 */
    FUNCTION_SELECT_USB_MUXING_OVERCURR_DETECT = 0x0a, /**< connect pin to USB muxing overcurrent detection */
    FUNCTION_SELECT_NULL                     = 0x1f,  /**< Reset value, pin unconnected */
} gpio_function_select_t;

/**
 * @brief   Get the PAD control register for the given GPIO pin as word
 *
 * Note: Only perform 32-bit writes to I/O registers.
 */
static inline volatile uint32_t * gpio_pad_register(uint8_t pin)
{
    return (uint32_t *)(PADS_BANK0_BASE + 4 + (pin << 2));
}

/**
 * @brief   Convenience function to set the pad configuration of the given pin
 *          using the bit-field convenience type
 */
static inline void gpio_set_pad_config(uint8_t pin, gpio_pad_ctrl_t config)
{
    uint32_t *c = (uint32_t *)&config;
    *gpio_pad_register(pin) = *c;
}

/**
 * @brief   Get the IO control register for the given GPIO pin as word
 */
static inline volatile uint32_t * gpio_io_register(uint8_t pin)
{
    return (uint32_t *)(IO_BANK0_BASE + 4 + (pin << 3));
}

/**
 * @brief   Convenience function to set the I/O configuration of the given pin
 *          using the bit-field convenience type
 */
static inline void gpio_set_io_config(uint8_t pin, gpio_io_ctrl_t config)
{
    uint32_t *c = (uint32_t *)&config;
    *gpio_io_register(pin) = *c;
}

/**
 * @brief   Reset value of the GPIO I/O configuration register
 */
#define GPIO_IO_REGISTER_RESET_VALUE    (0x0000001f)
/**
 * @brief   Reset value of the GPIO pad configuration register
 */
#define GPIO_PAD_REGISTER_RESET_VALUE    (0x00000056)

/**
 * @brief   Restore the default I/O and PAD configuration of the given GPIO pin
 */
static inline void gpio_reset_all_config(uint8_t pin)
{
    *gpio_io_register(pin) = GPIO_IO_REGISTER_RESET_VALUE;
    *gpio_pad_register(pin) = GPIO_PAD_REGISTER_RESET_VALUE;
}

#endif
