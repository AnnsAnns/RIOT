#ifndef UART_CONF_H
#define UART_CONF_H

#include "periph_cpu.h"
#include "macros/units.h"
#include "RP2350.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_UART 0
#define UART_TX_DEFAULT_PIN 16
#define UART_RX_DEFAULT_PIN 17
#define UART_NUMOF (2U)

#ifdef __cplusplus
}
#endif

#endif /* UART_CONF_H */
