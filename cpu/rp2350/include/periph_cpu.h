#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_PIN(x, y) ((unsigned int)(x + y))

/**
 * @brief UART module configuration options
 */
typedef struct {
    bool parity;
    unsigned int rx_pin;
    unsigned int tx_pin;
    uint8_t stop_bits;
    uint8_t data_bits;
} uart_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
