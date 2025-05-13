#ifndef CLOCK_CONF_H
#define CLOCK_CONF_H

#include "RP2350.h"

// Based on hardware/regs/xosc.h and 8.2.8
#define XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ    0xaa0u
#define XOSC_CTRL_FREQ_RANGE_VALUE_10_30MHZ     0xaa1u
#define XOSC_CTRL_FREQ_RANGE_VALUE_25_60MHZ    0xaa2u
#define XOSC_CTRL_FREQ_RANGE_VALUE_40_100MHZ    0xaa3u
#define XOSC_CTRL_ENABLE_VALUE_DISABLE 0xd1eu
#define XOSC_CTRL_ENABLE_VALUE_ENABLE 0xfabu
#define XOSC_CTRL_ENABLE_LSB    12u
#define XOSC_STATUS_STABLE_BITS   0x80000000u
/**
 * @brief   Configures the Crystal to run.
 *
 * @param   f_ref       Desired frequency in Hz
 *
 * @pre     1 MHz <= @p f_ref <= 15 MHz.
 *
 * The reference hardware manual suggests to use a 12 MHz crystal.
 */
void xosc_start(void);

/**
 * @brief   Stop the crystal.
 */
void xosc_stop(void);

/**
 * @brief   Sleep for a given number of cycles.
 */
void xosc_sleep(int32_t cycles);

#endif
