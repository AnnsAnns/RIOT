/*
 * SPDX-FileCopyrightText: 2026 RIOT contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_bao1x
 * @{
 *
 * @file
 * @brief       Internal definitions for the Baochip-1x USB device controller
 *
 * @}
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "periph/usbdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UDC register block. The register window starts at 0x50202000, the device
 * (xHCI) registers are at offset 0x400. */
#define BAO1X_USB_BASE              (0x50202000UL)
#define BAO1X_USB_REG(off)          (*(volatile uint32_t *)(BAO1X_USB_BASE + (off)))
#define BAO1X_REG32(addr)           (*(volatile uint32_t *)(uintptr_t)(addr))

#define USB_DEVCONFIG               (0x400 + 0x010)
#define USB_EVENTCONFIG             (0x400 + 0x014)
#define USB_USBCMD                  (0x400 + 0x020)
#define USB_USBSTS                  (0x400 + 0x024)
#define USB_DCBAPLO                 (0x400 + 0x028)
#define USB_DCBAPHI                 (0x400 + 0x02c)
#define USB_PORTSC                  (0x400 + 0x030)
#define USB_DOORBELL                (0x400 + 0x040)
#define USB_EPENABLE                (0x400 + 0x060)
#define USB_EPRUNNING               (0x400 + 0x064)
#define USB_CMDPARA0                (0x400 + 0x070)
#define USB_CMDPARA1                (0x400 + 0x074)
#define USB_CMDCTRL                 (0x400 + 0x078)
#define USB_IMAN                    (0x400 + 0x100)
#define USB_IMOD                    (0x400 + 0x104)
#define USB_ERSTSZ                  (0x400 + 0x108)
#define USB_ERSTBALO                (0x400 + 0x110)
#define USB_ERSTBAHI                (0x400 + 0x114)
#define USB_ERDPLO                  (0x400 + 0x118)
#define USB_ERDPHI                  (0x400 + 0x11c)
#define USB_U3PORTPMSC              (0x400 + 0x034)
#define USB_U2PORTPMSC              (0x400 + 0x038)

#define USBCMD_RUN_STOP             (1U << 0)
#define USBCMD_SOFT_RESET           (1U << 1)
#define USBCMD_INT_ENABLE           (1U << 2)
#define USBCMD_SYS_ERR_ENABLE       (1U << 3)

#define USBSTS_SYSTEM_ERR           (1U << 2)
#define USBSTS_EINT                 (1U << 3)

#define PORTSC_CCS                  (1U << 0)
#define PORTSC_PR                   (1U << 4)
#define PORTSC_CSC                  (1U << 17)
#define PORTSC_PRC                  (1U << 21)

#define EVENTCONFIG_CSC             (1U << 0)
#define EVENTCONFIG_PEC             (1U << 1)
#define EVENTCONFIG_PPC             (1U << 3)
#define EVENTCONFIG_PRC             (1U << 4)
#define EVENTCONFIG_PLC             (1U << 5)
#define EVENTCONFIG_CEC             (1U << 6)
#define EVENTCONFIG_INACTIVE_PLC    (1U << 12)
#define EVENTCONFIG_USB2_RESUME_PLC (1U << 14)
#define EVENTCONFIG_SETUP_ENABLE    (1U << 16)

#define DCBAP_MASK                  0xffffffc0UL
#define ERDP_EHB                    (1U << 3)

#define CMDCTRL_ACTIVE              (1U << 0)
#define CMDCTRL_TYPE_SHIFT          (4)
#define CMDCTRL_STATUS_SHIFT        (16)

#define CMD_INIT_EP0                (0)
#define CMD_UPDATE_EP0              (1)
#define CMD_SET_ADDRESS             (2)
#define CMD_CONFIG_EP               (4)
#define CMD_SET_HALT                (5)
#define CMD_CLEAR_HALT              (6)
#define CMD_RESET_SEQNUM            (7)
#define CMD_STOP_EP                 (8)
#define CMD_SET_TR_DQPTR            (9)

#define IMAN_IP                     (1U << 0)
#define IMAN_IE                     (1U << 1)

/* IRQARRAY1 (interrupt controller bank containing the USB IRQ) */
#define BAO1X_IRQARRAY1_BASE        (0xe0005000UL)
#define IRQARRAY_EV_PENDING         (0x10)
#define IRQARRAY_EV_ENABLE          (0x14)
#define BAO1X_USB_IRQARRAY_BIT      (0)

/* IOX register block, used to drive the board USB SE0/disconnect switch */
#define BAO1X_IOX_BASE              (0x5012f000UL)
#define IOX_GPIOOUT(port)           (0x130 + 4 * (port))
#define IOX_GPIOOE(port)            (0x148 + 4 * (port))
#define IOX_GPIOPU(port)            (0x160 + 4 * (port))
#define IOX_CFG_SCHM(port)          (0x230 + 4 * (port))
/* IFRAM area reserved for the UDC.  This is the top 23 pages of IFRAM, exactly
 * like the vendor bootloader uses it. */
#define UDC_MEM_BASE                (0x50009000UL)
#define UDC_MEM_LEN                 (0x17000UL)
#define UDC_MEM_END                 (UDC_MEM_BASE + UDC_MEM_LEN)

#define UDC_ERST_OFFSET             (0x0000)
#define UDC_EVENT_OFFSET            (0x0100)
#define UDC_EPCX_OFFSET             (0x0300)
#define UDC_EP0_RING_OFFSET         (0x0500)
#define UDC_EP_RINGS_OFFSET         (0x0600)

#define EVENT_RING_TRBS             (32)
#define EP0_RING_TRBS               (16)
#define EP_RING_TRBS                (64)
#define EP_RING_SLOTS               (8)     /* PEIs 2 .. 9 */
#define EP_RING_SIZE                (EP_RING_TRBS * sizeof(trb_t))

#define UDC_BOUNCE_OFFSET \
        (UDC_EP_RINGS_OFFSET + EP_RING_SLOTS * EP_RING_SIZE)
#define UDC_BOUNCE_LEN              (USBDEV_NUM_ENDPOINTS * 2 * 512)

#define MAX_EP0_PACKET              (64U)
#define MAX_BOUNCE_LEN              (512U)

#define TRB_TYPE_NORMAL             (1)
#define TRB_TYPE_DATA               (3)
#define TRB_TYPE_STATUS             (4)
#define TRB_TYPE_LINK               (6)
#define TRB_TYPE_XFER_EVENT         (32)
#define TRB_TYPE_CMD_EVENT          (33)
#define TRB_TYPE_PORT_EVENT         (34)
#define TRB_TYPE_SETUP_EVENT        (40)

#define COMP_STOPPED                (26)
#define COMP_STOPPED_LEN_INVALID    (27)
#define COMP_PROTOCOL_STALL         (192)
#define COMP_HALTED                 (194)
#define COMP_HALTED_LEN_INVALID     (195)
#define COMP_DISABLED               (196)

#define EP_EVT_NONE                 (0)
#define EP_EVT_XFER                 (1)

#define DEV_EVT_RESET               (1U << 0)
#define DEV_EVT_CONNECT             (1U << 1)
#define DEV_EVT_DISCONNECT          (1U << 2)
#define DEV_EVT_SUSPEND             (1U << 3)
#define DEV_EVT_RESUME              (1U << 4)

typedef struct __attribute__((packed)) {
    uint32_t dplo;
    uint32_t dphi;
    uint32_t dw2;
    uint32_t dw3;
} trb_t;

typedef struct __attribute__((packed)) {
    uint32_t seg_addr_lo;
    uint32_t seg_addr_hi;
    uint32_t seg_size;
    uint32_t rsvd;
} erst_t;

typedef struct __attribute__((packed)) {
    uint32_t dw0;
    uint32_t dw1;
    uint32_t dw2;
    uint32_t dw3;
} epcx_t;

typedef struct {
    trb_t *trbs;
    uint8_t count;
    uint8_t enq;
    bool pcs;
} ring_t;

typedef struct {
    usbdev_ep_t ep;
    ring_t ring;
    uint8_t *buf;
    uint8_t *dma;
    size_t len;
    size_t avail;
    uint8_t evt;
    uint8_t evt_type;
    uint16_t evt_residual;
    bool evt_is_status;
    bool configured;
    bool stalled;
    bool tx_pending;
} bao1x_usb_ep_t;

typedef struct {
    usbdev_t usbdev;
    uint8_t speed;
    bool started;
    volatile uint8_t dev_events;

    uint8_t setup[8];
    uint8_t setup_tag;
    bool setup_pending;
    bool have_setup;
    bool setup_read;
    uint16_t setup_len;
    uint16_t rx_len;
    uint16_t tx_len;
    bool status_set_addr;

    ring_t ep0_ring;
    epcx_t *epcx;

    volatile uint8_t evt_idx;
    volatile bool evt_ccs;

    bao1x_usb_ep_t eps[USBDEV_NUM_ENDPOINTS][2];
} bao1x_usb_t;

static inline bao1x_usb_t *_dev(usbdev_t *dev)
{
    return (bao1x_usb_t *)dev;
}

static inline bao1x_usb_ep_t *_bep(usbdev_ep_t *ep)
{
    return (bao1x_usb_ep_t *)ep;
}

static inline unsigned _pei(unsigned num, usb_ep_dir_t dir)
{
    return 2 * num + (dir == USB_EP_DIR_IN ? 0 : 1);
}

static inline void _barrier(void)
{
    __asm__ volatile ("fence rw, rw" ::: "memory");
}

static inline uint8_t *_bounce(unsigned num, usb_ep_dir_t dir)
{
    size_t index = (size_t)num * 2 + (dir == USB_EP_DIR_IN ? 1 : 0);

    return (uint8_t *)(UDC_MEM_BASE + UDC_BOUNCE_OFFSET +
                       index * MAX_BOUNCE_LEN);
}

static inline epcx_t *_epcx(bao1x_usb_t *dev, unsigned pei)
{
    return &dev->epcx[pei - 2];
}

/* Accessed from the IRQ handler and the Timer0 kick callback. */
extern bao1x_usb_t _usbdev;

/* low-level UDC core */
void bao1x_usb_preinit(void);
void bao1x_usb_attach(bool enable);
void bao1x_usb_hw_init(bao1x_usb_t *dev);
void bao1x_usb_ep0_hw_init(bao1x_usb_t *dev);
int bao1x_usb_cmd(uint32_t type, uint32_t param0, uint32_t param1);
void bao1x_usb_ring_init(ring_t *ring, void *mem, unsigned count);
trb_t *bao1x_usb_ring_get(ring_t *ring, bool *pcs);
void bao1x_usb_doorbell(unsigned pei);
void bao1x_usb_kick_start(void);

/* endpoint programming */
void bao1x_usb_ep_enable(bao1x_usb_t *dev, bao1x_usb_ep_t *bep);
void bao1x_usb_ep_disable(bao1x_usb_t *dev, bao1x_usb_ep_t *bep);
void bao1x_usb_ep_set_halt(bao1x_usb_t *dev, bao1x_usb_ep_t *bep,
                           bool enable);
void bao1x_usb_ep0_queue_data(bao1x_usb_t *dev, bool in, uint8_t *buf,
                              size_t len);
void bao1x_usb_ep0_queue_status(bao1x_usb_t *dev, bool in, bool stall,
                                bool set_addr);

#ifdef __cplusplus
}
#endif

/** @} */
