#ifndef TIMER_CONF_H
#define TIMER_CONF_H

#include "periph_cpu.h"
#include "RP2350.h"


#define PERIPH_TIMER_PROVIDES_SET

#define TIM_FLAG_RESET_ON_SET (0U)
#define TIM_FLAG_RESET_ON_MATCH (0U)

typedef struct {
    uint32_t period_us;
} timer_channel_conf_t;

typedef struct {
    bool is_running;
    timer_channel_conf_t channel[4];
} timer_conf_t;

/**
 * @brief CPU clock options
 */
uint32_t get_clk_khz(unsigned int clk_src_idx);

#endif /* TIMER_CONF_H */

