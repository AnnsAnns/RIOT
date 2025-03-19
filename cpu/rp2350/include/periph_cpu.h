#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdbool.h>
#include <stdint.h>
#include "RP2350.h"
#include "io_reg.h"

#define PROVIDES_PM_SET_LOWEST

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PICO_DEFAULT_UART
#define PICO_DEFAULT_UART 0
#endif
#ifndef PICO_DEFAULT_UART_TX_PIN
#define PICO_DEFAULT_UART_TX_PIN 16
#endif
#ifndef PICO_DEFAULT_UART_RX_PIN
#define PICO_DEFAULT_UART_RX_PIN 17
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


/* =========================================================================================================================== */
/* ================                                           UART0                                           ================ */
/* =========================================================================================================================== */

/* ========================================================  UARTDR  ========================================================= */
#define UART0_UARTDR_OE_Pos               (11UL)                    /*!< OE (Bit 11)                                           */
#define UART0_UARTDR_OE_Msk               (0x800UL)                 /*!< OE (Bitfield-Mask: 0x01)                              */
#define UART0_UARTDR_BE_Pos               (10UL)                    /*!< BE (Bit 10)                                           */
#define UART0_UARTDR_BE_Msk               (0x400UL)                 /*!< BE (Bitfield-Mask: 0x01)                              */
#define UART0_UARTDR_PE_Pos               (9UL)                     /*!< PE (Bit 9)                                            */
#define UART0_UARTDR_PE_Msk               (0x200UL)                 /*!< PE (Bitfield-Mask: 0x01)                              */
#define UART0_UARTDR_FE_Pos               (8UL)                     /*!< FE (Bit 8)                                            */
#define UART0_UARTDR_FE_Msk               (0x100UL)                 /*!< FE (Bitfield-Mask: 0x01)                              */
#define UART0_UARTDR_DATA_Pos             (0UL)                     /*!< DATA (Bit 0)                                          */
#define UART0_UARTDR_DATA_Msk             (0xffUL)                  /*!< DATA (Bitfield-Mask: 0xff)                            */
/* ========================================================  UARTRSR  ======================================================== */
#define UART0_UARTRSR_OE_Pos              (3UL)                     /*!< OE (Bit 3)                                            */
#define UART0_UARTRSR_OE_Msk              (0x8UL)                   /*!< OE (Bitfield-Mask: 0x01)                              */
#define UART0_UARTRSR_BE_Pos              (2UL)                     /*!< BE (Bit 2)                                            */
#define UART0_UARTRSR_BE_Msk              (0x4UL)                   /*!< BE (Bitfield-Mask: 0x01)                              */
#define UART0_UARTRSR_PE_Pos              (1UL)                     /*!< PE (Bit 1)                                            */
#define UART0_UARTRSR_PE_Msk              (0x2UL)                   /*!< PE (Bitfield-Mask: 0x01)                              */
#define UART0_UARTRSR_FE_Pos              (0UL)                     /*!< FE (Bit 0)                                            */
#define UART0_UARTRSR_FE_Msk              (0x1UL)                   /*!< FE (Bitfield-Mask: 0x01)                              */
/* ========================================================  UARTFR  ========================================================= */
#define UART0_UARTFR_RI_Pos               (8UL)                     /*!< RI (Bit 8)                                            */
#define UART0_UARTFR_RI_Msk               (0x100UL)                 /*!< RI (Bitfield-Mask: 0x01)                              */
#define UART0_UARTFR_TXFE_Pos             (7UL)                     /*!< TXFE (Bit 7)                                          */
#define UART0_UARTFR_TXFE_Msk             (0x80UL)                  /*!< TXFE (Bitfield-Mask: 0x01)                            */
#define UART0_UARTFR_RXFF_Pos             (6UL)                     /*!< RXFF (Bit 6)                                          */
#define UART0_UARTFR_RXFF_Msk             (0x40UL)                  /*!< RXFF (Bitfield-Mask: 0x01)                            */
#define UART0_UARTFR_TXFF_Pos             (5UL)                     /*!< TXFF (Bit 5)                                          */
#define UART0_UARTFR_TXFF_Msk             (0x20UL)                  /*!< TXFF (Bitfield-Mask: 0x01)                            */
#define UART0_UARTFR_RXFE_Pos             (4UL)                     /*!< RXFE (Bit 4)                                          */
#define UART0_UARTFR_RXFE_Msk             (0x10UL)                  /*!< RXFE (Bitfield-Mask: 0x01)                            */
#define UART0_UARTFR_BUSY_Pos             (3UL)                     /*!< BUSY (Bit 3)                                          */
#define UART0_UARTFR_BUSY_Msk             (0x8UL)                   /*!< BUSY (Bitfield-Mask: 0x01)                            */
#define UART0_UARTFR_DCD_Pos              (2UL)                     /*!< DCD (Bit 2)                                           */
#define UART0_UARTFR_DCD_Msk              (0x4UL)                   /*!< DCD (Bitfield-Mask: 0x01)                             */
#define UART0_UARTFR_DSR_Pos              (1UL)                     /*!< DSR (Bit 1)                                           */
#define UART0_UARTFR_DSR_Msk              (0x2UL)                   /*!< DSR (Bitfield-Mask: 0x01)                             */
#define UART0_UARTFR_CTS_Pos              (0UL)                     /*!< CTS (Bit 0)                                           */
#define UART0_UARTFR_CTS_Msk              (0x1UL)                   /*!< CTS (Bitfield-Mask: 0x01)                             */
/* =======================================================  UARTILPR  ======================================================== */
#define UART0_UARTILPR_ILPDVSR_Pos        (0UL)                     /*!< ILPDVSR (Bit 0)                                       */
#define UART0_UARTILPR_ILPDVSR_Msk        (0xffUL)                  /*!< ILPDVSR (Bitfield-Mask: 0xff)                         */
/* =======================================================  UARTIBRD  ======================================================== */
#define UART0_UARTIBRD_BAUD_DIVINT_Pos    (0UL)                     /*!< BAUD_DIVINT (Bit 0)                                   */
#define UART0_UARTIBRD_BAUD_DIVINT_Msk    (0xffffUL)                /*!< BAUD_DIVINT (Bitfield-Mask: 0xffff)                   */
/* =======================================================  UARTFBRD  ======================================================== */
#define UART0_UARTFBRD_BAUD_DIVFRAC_Pos   (0UL)                     /*!< BAUD_DIVFRAC (Bit 0)                                  */
#define UART0_UARTFBRD_BAUD_DIVFRAC_Msk   (0x3fUL)                  /*!< BAUD_DIVFRAC (Bitfield-Mask: 0x3f)                    */
/* =======================================================  UARTLCR_H  ======================================================= */
#define UART0_UARTLCR_H_SPS_Pos           (7UL)                     /*!< SPS (Bit 7)                                           */
#define UART0_UARTLCR_H_SPS_Msk           (0x80UL)                  /*!< SPS (Bitfield-Mask: 0x01)                             */
#define UART0_UARTLCR_H_WLEN_Pos          (5UL)                     /*!< WLEN (Bit 5)                                          */
#define UART0_UARTLCR_H_WLEN_Msk          (0x60UL)                  /*!< WLEN (Bitfield-Mask: 0x03)                            */
#define UART0_UARTLCR_H_FEN_Pos           (4UL)                     /*!< FEN (Bit 4)                                           */
#define UART0_UARTLCR_H_FEN_Msk           (0x10UL)                  /*!< FEN (Bitfield-Mask: 0x01)                             */
#define UART0_UARTLCR_H_STP2_Pos          (3UL)                     /*!< STP2 (Bit 3)                                          */
#define UART0_UARTLCR_H_STP2_Msk          (0x8UL)                   /*!< STP2 (Bitfield-Mask: 0x01)                            */
#define UART0_UARTLCR_H_EPS_Pos           (2UL)                     /*!< EPS (Bit 2)                                           */
#define UART0_UARTLCR_H_EPS_Msk           (0x4UL)                   /*!< EPS (Bitfield-Mask: 0x01)                             */
#define UART0_UARTLCR_H_PEN_Pos           (1UL)                     /*!< PEN (Bit 1)                                           */
#define UART0_UARTLCR_H_PEN_Msk           (0x2UL)                   /*!< PEN (Bitfield-Mask: 0x01)                             */
#define UART0_UARTLCR_H_BRK_Pos           (0UL)                     /*!< BRK (Bit 0)                                           */
#define UART0_UARTLCR_H_BRK_Msk           (0x1UL)                   /*!< BRK (Bitfield-Mask: 0x01)                             */
/* ========================================================  UARTCR  ========================================================= */
#define UART0_UARTCR_CTSEN_Pos            (15UL)                    /*!< CTSEN (Bit 15)                                        */
#define UART0_UARTCR_CTSEN_Msk            (0x8000UL)                /*!< CTSEN (Bitfield-Mask: 0x01)                           */
#define UART0_UARTCR_RTSEN_Pos            (14UL)                    /*!< RTSEN (Bit 14)                                        */
#define UART0_UARTCR_RTSEN_Msk            (0x4000UL)                /*!< RTSEN (Bitfield-Mask: 0x01)                           */
#define UART0_UARTCR_OUT2_Pos             (13UL)                    /*!< OUT2 (Bit 13)                                         */
#define UART0_UARTCR_OUT2_Msk             (0x2000UL)                /*!< OUT2 (Bitfield-Mask: 0x01)                            */
#define UART0_UARTCR_OUT1_Pos             (12UL)                    /*!< OUT1 (Bit 12)                                         */
#define UART0_UARTCR_OUT1_Msk             (0x1000UL)                /*!< OUT1 (Bitfield-Mask: 0x01)                            */
#define UART0_UARTCR_RTS_Pos              (11UL)                    /*!< RTS (Bit 11)                                          */
#define UART0_UARTCR_RTS_Msk              (0x800UL)                 /*!< RTS (Bitfield-Mask: 0x01)                             */
#define UART0_UARTCR_DTR_Pos              (10UL)                    /*!< DTR (Bit 10)                                          */
#define UART0_UARTCR_DTR_Msk              (0x400UL)                 /*!< DTR (Bitfield-Mask: 0x01)                             */
#define UART0_UARTCR_RXE_Pos              (9UL)                     /*!< RXE (Bit 9)                                           */
#define UART0_UARTCR_RXE_Msk              (0x200UL)                 /*!< RXE (Bitfield-Mask: 0x01)                             */
#define UART0_UARTCR_TXE_Pos              (8UL)                     /*!< TXE (Bit 8)                                           */
#define UART0_UARTCR_TXE_Msk              (0x100UL)                 /*!< TXE (Bitfield-Mask: 0x01)                             */
#define UART0_UARTCR_LBE_Pos              (7UL)                     /*!< LBE (Bit 7)                                           */
#define UART0_UARTCR_LBE_Msk              (0x80UL)                  /*!< LBE (Bitfield-Mask: 0x01)                             */
#define UART0_UARTCR_SIRLP_Pos            (2UL)                     /*!< SIRLP (Bit 2)                                         */
#define UART0_UARTCR_SIRLP_Msk            (0x4UL)                   /*!< SIRLP (Bitfield-Mask: 0x01)                           */
#define UART0_UARTCR_SIREN_Pos            (1UL)                     /*!< SIREN (Bit 1)                                         */
#define UART0_UARTCR_SIREN_Msk            (0x2UL)                   /*!< SIREN (Bitfield-Mask: 0x01)                           */
#define UART0_UARTCR_UARTEN_Pos           (0UL)                     /*!< UARTEN (Bit 0)                                        */
#define UART0_UARTCR_UARTEN_Msk           (0x1UL)                   /*!< UARTEN (Bitfield-Mask: 0x01)                          */
/* =======================================================  UARTIFLS  ======================================================== */
#define UART0_UARTIFLS_RXIFLSEL_Pos       (3UL)                     /*!< RXIFLSEL (Bit 3)                                      */
#define UART0_UARTIFLS_RXIFLSEL_Msk       (0x38UL)                  /*!< RXIFLSEL (Bitfield-Mask: 0x07)                        */
#define UART0_UARTIFLS_TXIFLSEL_Pos       (0UL)                     /*!< TXIFLSEL (Bit 0)                                      */
#define UART0_UARTIFLS_TXIFLSEL_Msk       (0x7UL)                   /*!< TXIFLSEL (Bitfield-Mask: 0x07)                        */
/* =======================================================  UARTIMSC  ======================================================== */
#define UART0_UARTIMSC_OEIM_Pos           (10UL)                    /*!< OEIM (Bit 10)                                         */
#define UART0_UARTIMSC_OEIM_Msk           (0x400UL)                 /*!< OEIM (Bitfield-Mask: 0x01)                            */
#define UART0_UARTIMSC_BEIM_Pos           (9UL)                     /*!< BEIM (Bit 9)                                          */
#define UART0_UARTIMSC_BEIM_Msk           (0x200UL)                 /*!< BEIM (Bitfield-Mask: 0x01)                            */
#define UART0_UARTIMSC_PEIM_Pos           (8UL)                     /*!< PEIM (Bit 8)                                          */
#define UART0_UARTIMSC_PEIM_Msk           (0x100UL)                 /*!< PEIM (Bitfield-Mask: 0x01)                            */
#define UART0_UARTIMSC_FEIM_Pos           (7UL)                     /*!< FEIM (Bit 7)                                          */
#define UART0_UARTIMSC_FEIM_Msk           (0x80UL)                  /*!< FEIM (Bitfield-Mask: 0x01)                            */
#define UART0_UARTIMSC_RTIM_Pos           (6UL)                     /*!< RTIM (Bit 6)                                          */
#define UART0_UARTIMSC_RTIM_Msk           (0x40UL)                  /*!< RTIM (Bitfield-Mask: 0x01)                            */
#define UART0_UARTIMSC_TXIM_Pos           (5UL)                     /*!< TXIM (Bit 5)                                          */
#define UART0_UARTIMSC_TXIM_Msk           (0x20UL)                  /*!< TXIM (Bitfield-Mask: 0x01)                            */
#define UART0_UARTIMSC_RXIM_Pos           (4UL)                     /*!< RXIM (Bit 4)                                          */
#define UART0_UARTIMSC_RXIM_Msk           (0x10UL)                  /*!< RXIM (Bitfield-Mask: 0x01)                            */
#define UART0_UARTIMSC_DSRMIM_Pos         (3UL)                     /*!< DSRMIM (Bit 3)                                        */
#define UART0_UARTIMSC_DSRMIM_Msk         (0x8UL)                   /*!< DSRMIM (Bitfield-Mask: 0x01)                          */
#define UART0_UARTIMSC_DCDMIM_Pos         (2UL)                     /*!< DCDMIM (Bit 2)                                        */
#define UART0_UARTIMSC_DCDMIM_Msk         (0x4UL)                   /*!< DCDMIM (Bitfield-Mask: 0x01)                          */
#define UART0_UARTIMSC_CTSMIM_Pos         (1UL)                     /*!< CTSMIM (Bit 1)                                        */
#define UART0_UARTIMSC_CTSMIM_Msk         (0x2UL)                   /*!< CTSMIM (Bitfield-Mask: 0x01)                          */
#define UART0_UARTIMSC_RIMIM_Pos          (0UL)                     /*!< RIMIM (Bit 0)                                         */
#define UART0_UARTIMSC_RIMIM_Msk          (0x1UL)                   /*!< RIMIM (Bitfield-Mask: 0x01)                           */
/* ========================================================  UARTRIS  ======================================================== */
#define UART0_UARTRIS_OERIS_Pos           (10UL)                    /*!< OERIS (Bit 10)                                        */
#define UART0_UARTRIS_OERIS_Msk           (0x400UL)                 /*!< OERIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTRIS_BERIS_Pos           (9UL)                     /*!< BERIS (Bit 9)                                         */
#define UART0_UARTRIS_BERIS_Msk           (0x200UL)                 /*!< BERIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTRIS_PERIS_Pos           (8UL)                     /*!< PERIS (Bit 8)                                         */
#define UART0_UARTRIS_PERIS_Msk           (0x100UL)                 /*!< PERIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTRIS_FERIS_Pos           (7UL)                     /*!< FERIS (Bit 7)                                         */
#define UART0_UARTRIS_FERIS_Msk           (0x80UL)                  /*!< FERIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTRIS_RTRIS_Pos           (6UL)                     /*!< RTRIS (Bit 6)                                         */
#define UART0_UARTRIS_RTRIS_Msk           (0x40UL)                  /*!< RTRIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTRIS_TXRIS_Pos           (5UL)                     /*!< TXRIS (Bit 5)                                         */
#define UART0_UARTRIS_TXRIS_Msk           (0x20UL)                  /*!< TXRIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTRIS_RXRIS_Pos           (4UL)                     /*!< RXRIS (Bit 4)                                         */
#define UART0_UARTRIS_RXRIS_Msk           (0x10UL)                  /*!< RXRIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTRIS_DSRRMIS_Pos         (3UL)                     /*!< DSRRMIS (Bit 3)                                       */
#define UART0_UARTRIS_DSRRMIS_Msk         (0x8UL)                   /*!< DSRRMIS (Bitfield-Mask: 0x01)                         */
#define UART0_UARTRIS_DCDRMIS_Pos         (2UL)                     /*!< DCDRMIS (Bit 2)                                       */
#define UART0_UARTRIS_DCDRMIS_Msk         (0x4UL)                   /*!< DCDRMIS (Bitfield-Mask: 0x01)                         */
#define UART0_UARTRIS_CTSRMIS_Pos         (1UL)                     /*!< CTSRMIS (Bit 1)                                       */
#define UART0_UARTRIS_CTSRMIS_Msk         (0x2UL)                   /*!< CTSRMIS (Bitfield-Mask: 0x01)                         */
#define UART0_UARTRIS_RIRMIS_Pos          (0UL)                     /*!< RIRMIS (Bit 0)                                        */
#define UART0_UARTRIS_RIRMIS_Msk          (0x1UL)                   /*!< RIRMIS (Bitfield-Mask: 0x01)                          */
/* ========================================================  UARTMIS  ======================================================== */
#define UART0_UARTMIS_OEMIS_Pos           (10UL)                    /*!< OEMIS (Bit 10)                                        */
#define UART0_UARTMIS_OEMIS_Msk           (0x400UL)                 /*!< OEMIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTMIS_BEMIS_Pos           (9UL)                     /*!< BEMIS (Bit 9)                                         */
#define UART0_UARTMIS_BEMIS_Msk           (0x200UL)                 /*!< BEMIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTMIS_PEMIS_Pos           (8UL)                     /*!< PEMIS (Bit 8)                                         */
#define UART0_UARTMIS_PEMIS_Msk           (0x100UL)                 /*!< PEMIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTMIS_FEMIS_Pos           (7UL)                     /*!< FEMIS (Bit 7)                                         */
#define UART0_UARTMIS_FEMIS_Msk           (0x80UL)                  /*!< FEMIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTMIS_RTMIS_Pos           (6UL)                     /*!< RTMIS (Bit 6)                                         */
#define UART0_UARTMIS_RTMIS_Msk           (0x40UL)                  /*!< RTMIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTMIS_TXMIS_Pos           (5UL)                     /*!< TXMIS (Bit 5)                                         */
#define UART0_UARTMIS_TXMIS_Msk           (0x20UL)                  /*!< TXMIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTMIS_RXMIS_Pos           (4UL)                     /*!< RXMIS (Bit 4)                                         */
#define UART0_UARTMIS_RXMIS_Msk           (0x10UL)                  /*!< RXMIS (Bitfield-Mask: 0x01)                           */
#define UART0_UARTMIS_DSRMMIS_Pos         (3UL)                     /*!< DSRMMIS (Bit 3)                                       */
#define UART0_UARTMIS_DSRMMIS_Msk         (0x8UL)                   /*!< DSRMMIS (Bitfield-Mask: 0x01)                         */
#define UART0_UARTMIS_DCDMMIS_Pos         (2UL)                     /*!< DCDMMIS (Bit 2)                                       */
#define UART0_UARTMIS_DCDMMIS_Msk         (0x4UL)                   /*!< DCDMMIS (Bitfield-Mask: 0x01)                         */
#define UART0_UARTMIS_CTSMMIS_Pos         (1UL)                     /*!< CTSMMIS (Bit 1)                                       */
#define UART0_UARTMIS_CTSMMIS_Msk         (0x2UL)                   /*!< CTSMMIS (Bitfield-Mask: 0x01)                         */
#define UART0_UARTMIS_RIMMIS_Pos          (0UL)                     /*!< RIMMIS (Bit 0)                                        */
#define UART0_UARTMIS_RIMMIS_Msk          (0x1UL)                   /*!< RIMMIS (Bitfield-Mask: 0x01)                          */
/* ========================================================  UARTICR  ======================================================== */
#define UART0_UARTICR_OEIC_Pos            (10UL)                    /*!< OEIC (Bit 10)                                         */
#define UART0_UARTICR_OEIC_Msk            (0x400UL)                 /*!< OEIC (Bitfield-Mask: 0x01)                            */
#define UART0_UARTICR_BEIC_Pos            (9UL)                     /*!< BEIC (Bit 9)                                          */
#define UART0_UARTICR_BEIC_Msk            (0x200UL)                 /*!< BEIC (Bitfield-Mask: 0x01)                            */
#define UART0_UARTICR_PEIC_Pos            (8UL)                     /*!< PEIC (Bit 8)                                          */
#define UART0_UARTICR_PEIC_Msk            (0x100UL)                 /*!< PEIC (Bitfield-Mask: 0x01)                            */
#define UART0_UARTICR_FEIC_Pos            (7UL)                     /*!< FEIC (Bit 7)                                          */
#define UART0_UARTICR_FEIC_Msk            (0x80UL)                  /*!< FEIC (Bitfield-Mask: 0x01)                            */
#define UART0_UARTICR_RTIC_Pos            (6UL)                     /*!< RTIC (Bit 6)                                          */
#define UART0_UARTICR_RTIC_Msk            (0x40UL)                  /*!< RTIC (Bitfield-Mask: 0x01)                            */
#define UART0_UARTICR_TXIC_Pos            (5UL)                     /*!< TXIC (Bit 5)                                          */
#define UART0_UARTICR_TXIC_Msk            (0x20UL)                  /*!< TXIC (Bitfield-Mask: 0x01)                            */
#define UART0_UARTICR_RXIC_Pos            (4UL)                     /*!< RXIC (Bit 4)                                          */
#define UART0_UARTICR_RXIC_Msk            (0x10UL)                  /*!< RXIC (Bitfield-Mask: 0x01)                            */
#define UART0_UARTICR_DSRMIC_Pos          (3UL)                     /*!< DSRMIC (Bit 3)                                        */
#define UART0_UARTICR_DSRMIC_Msk          (0x8UL)                   /*!< DSRMIC (Bitfield-Mask: 0x01)                          */
#define UART0_UARTICR_DCDMIC_Pos          (2UL)                     /*!< DCDMIC (Bit 2)                                        */
#define UART0_UARTICR_DCDMIC_Msk          (0x4UL)                   /*!< DCDMIC (Bitfield-Mask: 0x01)                          */
#define UART0_UARTICR_CTSMIC_Pos          (1UL)                     /*!< CTSMIC (Bit 1)                                        */
#define UART0_UARTICR_CTSMIC_Msk          (0x2UL)                   /*!< CTSMIC (Bitfield-Mask: 0x01)                          */
#define UART0_UARTICR_RIMIC_Pos           (0UL)                     /*!< RIMIC (Bit 0)                                         */
#define UART0_UARTICR_RIMIC_Msk           (0x1UL)                   /*!< RIMIC (Bitfield-Mask: 0x01)                           */
/* =======================================================  UARTDMACR  ======================================================= */
#define UART0_UARTDMACR_DMAONERR_Pos      (2UL)                     /*!< DMAONERR (Bit 2)                                      */
#define UART0_UARTDMACR_DMAONERR_Msk      (0x4UL)                   /*!< DMAONERR (Bitfield-Mask: 0x01)                        */
#define UART0_UARTDMACR_TXDMAE_Pos        (1UL)                     /*!< TXDMAE (Bit 1)                                        */
#define UART0_UARTDMACR_TXDMAE_Msk        (0x2UL)                   /*!< TXDMAE (Bitfield-Mask: 0x01)                          */
#define UART0_UARTDMACR_RXDMAE_Pos        (0UL)                     /*!< RXDMAE (Bit 0)                                        */
#define UART0_UARTDMACR_RXDMAE_Msk        (0x1UL)                   /*!< RXDMAE (Bitfield-Mask: 0x01)                          */
/* =====================================================  UARTPERIPHID0  ===================================================== */
#define UART0_UARTPERIPHID0_PARTNUMBER0_Pos (0UL)                   /*!< PARTNUMBER0 (Bit 0)                                   */
#define UART0_UARTPERIPHID0_PARTNUMBER0_Msk (0xffUL)                /*!< PARTNUMBER0 (Bitfield-Mask: 0xff)                     */
/* =====================================================  UARTPERIPHID1  ===================================================== */
#define UART0_UARTPERIPHID1_DESIGNER0_Pos (4UL)                     /*!< DESIGNER0 (Bit 4)                                     */
#define UART0_UARTPERIPHID1_DESIGNER0_Msk (0xf0UL)                  /*!< DESIGNER0 (Bitfield-Mask: 0x0f)                       */
#define UART0_UARTPERIPHID1_PARTNUMBER1_Pos (0UL)                   /*!< PARTNUMBER1 (Bit 0)                                   */
#define UART0_UARTPERIPHID1_PARTNUMBER1_Msk (0xfUL)                 /*!< PARTNUMBER1 (Bitfield-Mask: 0x0f)                     */
/* =====================================================  UARTPERIPHID2  ===================================================== */
#define UART0_UARTPERIPHID2_REVISION_Pos  (4UL)                     /*!< REVISION (Bit 4)                                      */
#define UART0_UARTPERIPHID2_REVISION_Msk  (0xf0UL)                  /*!< REVISION (Bitfield-Mask: 0x0f)                        */
#define UART0_UARTPERIPHID2_DESIGNER1_Pos (0UL)                     /*!< DESIGNER1 (Bit 0)                                     */
#define UART0_UARTPERIPHID2_DESIGNER1_Msk (0xfUL)                   /*!< DESIGNER1 (Bitfield-Mask: 0x0f)                       */
/* =====================================================  UARTPERIPHID3  ===================================================== */
#define UART0_UARTPERIPHID3_CONFIGURATION_Pos (0UL)                 /*!< CONFIGURATION (Bit 0)                                 */
#define UART0_UARTPERIPHID3_CONFIGURATION_Msk (0xffUL)              /*!< CONFIGURATION (Bitfield-Mask: 0xff)                   */
/* =====================================================  UARTPCELLID0  ====================================================== */
#define UART0_UARTPCELLID0_UARTPCELLID0_Pos (0UL)                   /*!< UARTPCELLID0 (Bit 0)                                  */
#define UART0_UARTPCELLID0_UARTPCELLID0_Msk (0xffUL)                /*!< UARTPCELLID0 (Bitfield-Mask: 0xff)                    */
/* =====================================================  UARTPCELLID1  ====================================================== */
#define UART0_UARTPCELLID1_UARTPCELLID1_Pos (0UL)                   /*!< UARTPCELLID1 (Bit 0)                                  */
#define UART0_UARTPCELLID1_UARTPCELLID1_Msk (0xffUL)                /*!< UARTPCELLID1 (Bitfield-Mask: 0xff)                    */
/* =====================================================  UARTPCELLID2  ====================================================== */
#define UART0_UARTPCELLID2_UARTPCELLID2_Pos (0UL)                   /*!< UARTPCELLID2 (Bit 0)                                  */
#define UART0_UARTPCELLID2_UARTPCELLID2_Msk (0xffUL)                /*!< UARTPCELLID2 (Bitfield-Mask: 0xff)                    */
/* =====================================================  UARTPCELLID3  ====================================================== */
#define UART0_UARTPCELLID3_UARTPCELLID3_Pos (0UL)                   /*!< UARTPCELLID3 (Bit 0)                                  */
#define UART0_UARTPCELLID3_UARTPCELLID3_Msk (0xffUL)                /*!< UARTPCELLID3 (Bitfield-Mask: 0xff)                    */


#endif /* PERIPH_CPU_H */
/** @} */
