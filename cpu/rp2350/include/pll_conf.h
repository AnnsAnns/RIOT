#ifndef __PLL_CONF_H__
#define __PLL_CONF_H__

#include "macros/units.h"

#if !defined(PLL_SYS_REF_DIV) || defined(DOXYGEN)
/**
 * @brief   For generating the system clock via PLL, the XOSC reference clock can be divided. The
 *          datasheet says for the supported frequency range of the crystal, only a divider of 1 is
 *          reasonable.
 */
#define PLL_SYS_REF_DIV         1
#endif

#if !defined(PLL_USB_REF_DIV) || defined(DOXYGEN)
/**
 * @brief   Same as @ref PLL_SYS_REF_DIV but for the PLL generating the USB clock
 */
#define PLL_USB_REF_DIV         1
#endif

#if !defined(PLL_SYS_VCO_FEEDBACK_SCALE) || defined(DOXYGEN)
/**
 * @brief   VCO feedback scale used for system clock
 *
 * @note    The result of `PLL_SYS_VCO_FEEDBACK_SCALE * CLOCK_XOSC` must be in the range of
 *          400 MHz to 1600 MHz
 */
#define PLL_SYS_VCO_FEEDBACK_SCALE  125
#endif

#if !defined(PLL_SYS_POSTDIV1) || defined(DOXYGEN)
/**
 * @brief   First post-PLL clock divider for system clock
 *
 * @note    Must be in rage 1..7
 */
#define PLL_SYS_POSTDIV1            6
#endif

#if !defined(PLL_SYS_POSTDIV2) || defined(DOXYGEN)
/**
 * @brief   Second post-PLL clock divider for system clock
 *
 * @note    Must be in rage 1..7
 */
#define PLL_SYS_POSTDIV2            2
#endif

#if !defined(PLL_USB_VCO_FEEDBACK_SCALE) || defined(DOXYGEN)
/**
 * @brief   VCO feedback scale used for USB clock
 *
 * @note    The result of `PLL_USB_VCO_FEEDBACK_SCALE * CLOCK_XOSC` must be in the range of
 *          400 MHz to 1600 MHz
 */
#define PLL_USB_VCO_FEEDBACK_SCALE  40
#endif

#if !defined(PLL_USB_POSTDIV1) || defined(DOXYGEN)
/**
 * @brief   First post-PLL clock divider for USB clock
 *
 * @note    Must be in rage 1..7
 */
#define PLL_USB_POSTDIV1            5
#endif

#if !defined(PLL_USB_POSTDIV2) || defined(DOXYGEN)
/**
 * @brief   Second post-PLL clock divider for USB clock
 *
 * @note    Must be in rage 1..7
 */
#define PLL_USB_POSTDIV2            2
#endif

#if !defined(CLOCK_XOSC) || defined(DOXYGEN)
/**
 * @brief   The RP2040 reference design attaches a 12 MHz crystal to the MCU, so we take this
 *          value as default
 */
#define CLOCK_XOSC          MHZ(12)
#endif

/**
 * @brief   Calculate the resulting PLL clock frequency for the given parameters
 */
#define PLL_CLOCK(vco_feedback, postdiv1, postdiv2) \
    (CLOCK_XOSC * (vco_feedback) / (postdiv1) / (postdiv2))

/**
 * @brief   System core clock speed, 125 MHz unless board changes parameters
 */
#define CLOCK_CORECLOCK \
    PLL_CLOCK(PLL_SYS_VCO_FEEDBACK_SCALE, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2)

/**
 * @brief   USB clock speed
 */
#define CLOCK_USB \
    PLL_CLOCK(PLL_USB_VCO_FEEDBACK_SCALE, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2)

/**
 * @name    Ranges for clock frequencies and clock settings
 * @{
 */
#define CLOCK_XOSC_MAX              MHZ(15) /**< Maximum crystal frequency */
#define CLOCK_XOSC_MIN              MHZ(5)  /**< Minimum crystal frequency */
#define PLL_POSTDIV_MIN             (1U)    /**< Minimum value of the post PLL clock divers */
#define PLL_POSTDIV_MAX             (7U)    /**< Maximum value of the post PLL clock divers */
#define PLL_VCO_FEEDBACK_SCALE_MIN  (16U)   /**< Minimum value of the PLL VCO feedback scaler */
#define PLL_VCO_FEEDBACK_SCALE_MAX  (320U)  /**< Maximum value of the PLL VCO feedback scaler */
#define PLL_REF_DIV_MIN             (1U)    /**< Minimum value of the clock divider applied before
                                             *   feeding in the reference clock into the PLL */
#define PLL_REF_DIV_MAX             (1U)    /**< Minimum value of the clock divider applied before
                                             *   feeding in the reference clock into the PLL */
/** @} */

#if CLOCK_USB != MHZ(48)
#error "USB clock != 48 MHz, check PLL_USB_VCO_FEEDBACK_SCALE, PLL_USB_POSTDIV1, PLL_SYS_POSTDIV2"
#endif

#if (CLOCK_XOSC > CLOCK_XOSC_MAX) || (CLOCK_XOSC < CLOCK_XOSC_MIN)
#error "Value for CLOCK_XOSC out of range, check config"
#endif

#if (PLL_SYS_REF_DIV < PLL_REF_DIV_MIN) || (PLL_SYS_REF_DIV > PLL_REF_DIV_MAX)
#error "Value for PLL_SYS_REF_DIV out of range, check config"
#endif

#if (PLL_USB_REF_DIV < PLL_REF_DIV_MIN) || (PLL_USB_REF_DIV > PLL_REF_DIV_MAX)
#error "Value for PLL_USB_REF_DIV out of range, check config"
#endif

#if (PLL_SYS_VCO_FEEDBACK_SCALE < PLL_VCO_FEEDBACK_SCALE_MIN) \
    || (PLL_SYS_VCO_FEEDBACK_SCALE > PLL_VCO_FEEDBACK_SCALE_MAX)
#error "Value for PLL_SYS_VCO_FEEDBACK_SCALE out of range, check config"
#endif

#if (PLL_USB_VCO_FEEDBACK_SCALE < PLL_VCO_FEEDBACK_SCALE_MIN) \
    || (PLL_USB_VCO_FEEDBACK_SCALE > PLL_VCO_FEEDBACK_SCALE_MAX)
#error "Value for PLL_USB_VCO_FEEDBACK_SCALE out of range, check config"
#endif

#if (PLL_SYS_POSTDIV1 < PLL_POSTDIV_MIN) || (PLL_SYS_POSTDIV1 > PLL_POSTDIV_MAX)
#error "Value for PLL_SYS_POSTDIV1 out of range, check config"
#endif

#if (PLL_SYS_POSTDIV2 < PLL_POSTDIV_MIN) || (PLL_SYS_POSTDIV2 > PLL_POSTDIV_MAX)
#error "Value for PLL_SYS_POSTDIV2 out of range, check config"
#endif

#if (PLL_USB_POSTDIV1 < PLL_POSTDIV_MIN) || (PLL_USB_POSTDIV1 > PLL_POSTDIV_MAX)
#error "Value for PLL_USB_POSTDIV1 out of range, check config"
#endif

#if (PLL_USB_POSTDIV2 < PLL_POSTDIV_MIN) || (PLL_USB_POSTDIV2 > PLL_POSTDIV_MAX)
#error "Value for PLL_USB_POSTDIV2 out of range, check config"
#endif

#if !defined(CLOCK_PERIPH_SOURCE) || defined(DOXYGEN)
/**
 * @brief   How to source the peripheral clock (default: 125 MHz system clock)
 */
#define CLOCK_PERIPH_SOURCE CLOCKS_CLK_PERI_CTRL_AUXSRC_clk_sys
#endif

#if !defined(CLOCK_PERIPH) || defined(DOXYGEN)
/**
 * @brief   Peripheral clock (by default source from system clock)
 */
#define CLOCK_PERIPH        CLOCK_CORECLOCK
#endif

#endif /* __PLL_CONF_H__ */
