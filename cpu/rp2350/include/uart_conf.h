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

// Reset bit masks for RESETS->RESET register
#define RESETS_RESET_uart0_Msk          (1u << 22)  // UART0 reset bit
#define RESETS_RESET_uart1_Msk          (1u << 23)  // UART1 reset bit

// UART register bit masks
#define UART0_UARTDR_OE_Msk             (1u << 11)  // Overrun error mask
#define UART0_UARTDR_BE_Msk             (1u << 10)  // Break error mask
#define UART0_UARTDR_PE_Msk             (1u << 9)   // Parity error mask
#define UART0_UARTDR_FE_Msk             (1u << 8)   // Framing error mask

#define UART0_UARTFR_TXFF_Msk           (1u << 5)   // Transmit FIFO full
#define UART0_UARTFR_RXFE_Msk           (1u << 4)   // Receive FIFO empty

#define UART0_UARTLCR_H_WLEN_Pos        5u          // Word length position
#define UART0_UARTLCR_H_STP2_Msk        (1u << 3)   // Two stop bits mask
#define UART0_UARTLCR_H_EPS_Msk         (1u << 2)   // Even parity select
#define UART0_UARTLCR_H_PEN_Msk         (1u << 1)   // Parity enable

#define UART0_UARTCR_UARTEN_Msk         (1u << 0)   // UART enable
#define UART0_UARTCR_TXE_Msk            (1u << 8)   // Transmit enable
#define UART0_UARTCR_RXE_Msk            (1u << 9)   // Receive enable

#define UART0_UARTIMSC_RXIM_Msk         (1u << 4)   // Receive interrupt mask
#define UART0_UARTIMSC_TXIM_Msk         (1u << 5)   // Transmit interrupt mask

#define UART0_UARTRIS_RXRIS_Msk         (1u << 4)   // Receive interrupt status
#define UART0_UARTRIS_TXRIS_Msk         (1u << 5)   // Transmit interrupt status

#define UART0_UARTMIS_RXMIS_Msk         (1u << 4)   // Receive masked interrupt status
#define UART0_UARTMIS_TXMIS_Msk         (1u << 5)   // Transmit masked interrupt status

#define UART0_UARTICR_RXIC_Msk          (1u << 4)   // Receive interrupt clear
#define UART0_UARTICR_TXIC_Msk          (1u << 5)   // Transmit interrupt clear
#ifdef __cplusplus
}
#endif

#endif /* UART_CONF_H */
