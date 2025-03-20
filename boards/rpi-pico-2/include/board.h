#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED configuration
 * @{
 */
#define LED0_PIN        GPIO_PIN(0, 25)
#define LED0_ON         do {SIO->GPIO_OUT_SET = 1UL << 25;} while (0)
#define LED0_OFF        do {SIO->GPIO_OUT_CLR = 1UL << 25;} while (0)
#define LED0_TOGGLE     do {SIO->GPIO_OUT_XOR = 1UL << 25;} while (0)
#define LED0_NAME       "LED(Green)"
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
