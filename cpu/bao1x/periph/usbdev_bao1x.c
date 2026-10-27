/*
 * SPDX-FileCopyrightText: 2026 RIOT contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_bao1x
 * @{
 *
 * @file
 * @brief       Low-level UDC core for the Baochip-1x USB device controller
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "usbdev_bao1x.h"

#include "bao1x_irq.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEVCONFIG_USB3_DISABLE      (1U << 7)
#define DEVCONFIG_MAX_SPEED_FS      (1U)

/* Timer0: 32-bit down-counter with a reload event.  It is only used here to
 * re-ring a pending OUT doorbell, see @ref bao1x_usb_kick_start. */
#define TIMER0_BASE                 (0xe001c000UL)
#define TIMER0_LOAD                 (TIMER0_BASE + 0x00)
#define TIMER0_RELOAD               (TIMER0_BASE + 0x04)
#define TIMER0_EN                   (TIMER0_BASE + 0x08)
#define TIMER0_EV_PENDING           (TIMER0_BASE + 0x18)
#define TIMER0_EV_ENABLE            (TIMER0_BASE + 0x1c)

static volatile bool _kick_active;

static bool _trb_ptr_valid(const trb_t *trb)
{
    uintptr_t ptr = (uintptr_t)trb;

    return (ptr >= UDC_MEM_BASE) && (ptr + sizeof(trb_t) <= UDC_MEM_END) &&
           ((ptr & 0xf) == 0);
}

static void _delay(void)
{
    /* The boot ROM does not give us a usable timer here yet.  A few million
     * NOPS are enough to keep the USB port in SE0 long enough for the host to
     * notice the disconnect. */
    for (volatile unsigned i = 0; i < 12000000U; i++) {
        __asm__ volatile ("nop");
    }
}

static void _se0(uint32_t value)
{
#ifdef BOARD_USB_SE0_PORT
    volatile uint32_t *oe = (volatile uint32_t *)(BAO1X_IOX_BASE +
                                                  IOX_GPIOOE(BOARD_USB_SE0_PORT));
    volatile uint32_t *out = (volatile uint32_t *)(BAO1X_IOX_BASE +
                                                   IOX_GPIOOUT(BOARD_USB_SE0_PORT));
    volatile uint32_t *pu = (volatile uint32_t *)(BAO1X_IOX_BASE +
                                                  IOX_GPIOPU(BOARD_USB_SE0_PORT));
    volatile uint32_t *schm = (volatile uint32_t *)(BAO1X_IOX_BASE +
                                                    IOX_CFG_SCHM(BOARD_USB_SE0_PORT));

    *oe |= (1U << BOARD_USB_SE0_PIN);
    *schm |= (1U << BOARD_USB_SE0_PIN);
    *pu |= (1U << BOARD_USB_SE0_PIN);
    if (value) {
        *out |= (1U << BOARD_USB_SE0_PIN);
    }
    else {
        *out &= ~(1U << BOARD_USB_SE0_PIN);
    }
#else
    (void)value;
#endif
}

static void _se0_release(void)
{
#ifdef BOARD_USB_SE0_PORT
    /* Let the pull-up de-assert SE0 and turn the pad into an input again so the
     * boot button keeps working. */
    volatile uint32_t *oe = (volatile uint32_t *)(BAO1X_IOX_BASE +
                                                  IOX_GPIOOE(BOARD_USB_SE0_PORT));
    volatile uint32_t *out = (volatile uint32_t *)(BAO1X_IOX_BASE +
                                                   IOX_GPIOOUT(BOARD_USB_SE0_PORT));
    volatile uint32_t *pu = (volatile uint32_t *)(BAO1X_IOX_BASE +
                                                  IOX_GPIOPU(BOARD_USB_SE0_PORT));

    *out |= (1U << BOARD_USB_SE0_PIN);
    *oe &= ~(1U << BOARD_USB_SE0_PIN);
    *pu |= (1U << BOARD_USB_SE0_PIN);
#endif
}

int bao1x_usb_cmd(uint32_t type, uint32_t param0, uint32_t param1)
{
    unsigned guard = 0;

    _barrier();

    while (BAO1X_USB_REG(USB_CMDCTRL) & CMDCTRL_ACTIVE) {
        if (++guard > 1000000U) {
            return -ETIMEDOUT;
        }
    }

    BAO1X_USB_REG(USB_CMDPARA0) = param0;
    BAO1X_USB_REG(USB_CMDPARA1) = param1;
    _barrier();
    BAO1X_USB_REG(USB_CMDCTRL) = (type << CMDCTRL_TYPE_SHIFT) | CMDCTRL_ACTIVE;

    guard = 0;
    while (BAO1X_USB_REG(USB_CMDCTRL) & CMDCTRL_ACTIVE) {
        if (++guard > 1000000U) {
            return -ETIMEDOUT;
        }
    }

    if ((BAO1X_USB_REG(USB_CMDCTRL) >> CMDCTRL_STATUS_SHIFT) & 0xf) {
        return -EIO;
    }
    return 0;
}

void bao1x_usb_ring_init(ring_t *ring, void *mem, unsigned count)
{
    memset(mem, 0, count * sizeof(trb_t));
    ring->trbs = mem;
    ring->count = count;
    ring->enq = 0;
    ring->pcs = true;

    trb_t *link = &ring->trbs[count - 1];
    link->dplo = (uintptr_t)ring->trbs;
    link->dw3 = (1U << 1) | ((uint32_t)TRB_TYPE_LINK << 10);
}

trb_t *bao1x_usb_ring_get(ring_t *ring, bool *pcs)
{
    unsigned index = ring->enq;

    *pcs = ring->pcs;

    ring->enq++;
    if (ring->enq == ring->count - 1) {
        trb_t *link = &ring->trbs[ring->count - 1];
        link->dw3 = (link->dw3 & ~1U) | (ring->pcs ? 1U : 0U);
        ring->pcs = !ring->pcs;
        ring->enq = 0;
    }

    return &ring->trbs[index];
}

void bao1x_usb_doorbell(unsigned pei)
{
    _barrier();
    BAO1X_USB_REG(USB_DOORBELL) = pei;
    _barrier();
}

/* The UDC only latches an OUT doorbell if it is written after the host has
 * already queued data.  Keep Timer0 running at 1 kHz while an OUT transfer is
 * pending and re-ring its doorbell. */
void bao1x_usb_kick_start(void)
{
    if (!_kick_active) {
        BAO1X_REG32(TIMER0_EN) = 0;
        BAO1X_REG32(TIMER0_EV_ENABLE) = 0;
        BAO1X_REG32(TIMER0_EV_PENDING) = 1;
        BAO1X_REG32(TIMER0_LOAD) = 350000U; /* 1 ms at ACLK = 350 MHz */
        BAO1X_REG32(TIMER0_RELOAD) = 350000U;
        BAO1X_REG32(TIMER0_EV_ENABLE) = 1;
        bao1x_irq_enable(BAO1X_USB_TIMER_IRQ);
        BAO1X_REG32(TIMER0_EN) = 1;
        _kick_active = true;
    }
}

void bao1x_usb_timer_isr(void)
{
    bool pending = false;

    for (unsigned num = 1; num < USBDEV_NUM_ENDPOINTS; num++) {
        bao1x_usb_ep_t *bep = &_usbdev.eps[num][USB_EP_DIR_OUT];
        if (bep->tx_pending && bep->configured) {
            pending = true;
            bao1x_usb_doorbell(_pei(num, USB_EP_DIR_OUT));
        }
    }

    if (!pending) {
        BAO1X_REG32(TIMER0_EN) = 0;
        BAO1X_REG32(TIMER0_EV_ENABLE) = 0;
        _kick_active = false;
    }
}

void bao1x_usb_ep0_hw_init(bao1x_usb_t *dev)
{
    bao1x_usb_ring_init(&dev->ep0_ring,
                        (void *)(UDC_MEM_BASE + UDC_EP0_RING_OFFSET),
                        EP0_RING_TRBS);

    uint32_t dqptr = ((uintptr_t)dev->ep0_ring.trbs & ~0xfUL);
    if (bao1x_usb_cmd(CMD_INIT_EP0, dqptr | (dev->ep0_ring.pcs ? 1U : 0U), 0) < 0) {
        DEBUG("bao1x_usb: INIT_EP0 failed\n");
    }
    if (bao1x_usb_cmd(CMD_UPDATE_EP0, MAX_EP0_PACKET << 16, 0) < 0) {
        DEBUG("bao1x_usb: UPDATE_EP0 failed\n");
    }

    dev->have_setup = false;
    dev->setup_pending = false;
    dev->setup_len = 0;
    dev->rx_len = 0;
    dev->tx_len = 0;
    dev->status_set_addr = false;
    BAO1X_USB_REG(USB_EVENTCONFIG) |= EVENTCONFIG_SETUP_ENABLE;
}

void bao1x_usb_preinit(void)
{
    _se0(0);
    _delay();
}

void bao1x_usb_attach(bool enable)
{
    if (enable) {
        _se0_release();
    }
    else {
        _se0(0);
    }
}

/* Register values required by the vendor reference driver.  These configure
 * internal PHY/UDC details that are not part of the public register map. */
static void _hw_reset(void)
{
    static const uint32_t magic[] = {
        0x0fc, 0x00000001,
        0x084, 0x01401388,
        0x0f4, 0x0000f023,
        0x088, 0x3b066409,
        0x08c, 0x0d020407,
        0x090, 0x04055050,
        0x094, 0x03030a07,
        0x098, 0x05131304,
        0x09c, 0x3b4b0d15,
        0x0a0, 0x14168c6e,
        0x0a4, 0x18060408,
        0x0a8, 0x4b120c0f,
        0x0ac, 0x03190d05,
        0x0b0, 0x08080d09,
        0x0b4, 0x20060b03,
        0x0b8, 0x040a8c0e,
        0x0bc, 0x44087d5a,
        0x110, 0x00000000,
    };

    for (unsigned i = 0; i < sizeof(magic) / sizeof(magic[0]); i += 2) {
        BAO1X_USB_REG(magic[i]) = magic[i + 1];
    }

    BAO1X_USB_REG(USB_USBCMD) &= ~(USBCMD_INT_ENABLE | USBCMD_RUN_STOP);
    BAO1X_USB_REG(USB_USBCMD) |= USBCMD_SOFT_RESET;
    for (unsigned i = 0; i < 10000000U; i++) {
        if (!(BAO1X_USB_REG(USB_USBCMD) & USBCMD_SOFT_RESET)) {
            return;
        }
    }
}

void bao1x_usb_hw_init(bao1x_usb_t *dev)
{
    _hw_reset();

    memset((void *)UDC_MEM_BASE, 0, UDC_MEM_LEN);
    dev->epcx = (epcx_t *)(UDC_MEM_BASE + UDC_EPCX_OFFSET);
    dev->evt_idx = 0;
    dev->evt_ccs = true;

    /* Full speed: the CDC ACM bulk endpoints are 64 byte and the RIOT USBUS
     * CDC ACM implementation is built around that size. */
    BAO1X_USB_REG(USB_DEVCONFIG) = DEVCONFIG_USB3_DISABLE |
                                   DEVCONFIG_MAX_SPEED_FS;

    BAO1X_USB_REG(USB_EVENTCONFIG) = EVENTCONFIG_CSC |
                                     EVENTCONFIG_PEC |
                                     EVENTCONFIG_PPC |
                                     EVENTCONFIG_PRC |
                                     EVENTCONFIG_PLC |
                                     EVENTCONFIG_CEC |
                                     EVENTCONFIG_INACTIVE_PLC |
                                     EVENTCONFIG_USB2_RESUME_PLC;

    erst_t *erst = (erst_t *)(UDC_MEM_BASE + UDC_ERST_OFFSET);
    erst->seg_addr_lo = UDC_MEM_BASE + UDC_EVENT_OFFSET;
    erst->seg_addr_hi = 0;
    erst->seg_size = EVENT_RING_TRBS;
    erst->rsvd = 0;

    BAO1X_USB_REG(USB_ERSTSZ) = 1;
    BAO1X_USB_REG(USB_ERSTBALO) = (uintptr_t)erst;
    BAO1X_USB_REG(USB_ERSTBAHI) = 0;
    BAO1X_USB_REG(USB_ERDPLO) =
        ((UDC_MEM_BASE + UDC_EVENT_OFFSET) & ~0xfUL) | ERDP_EHB;
    BAO1X_USB_REG(USB_ERDPHI) = 0;

    BAO1X_USB_REG(USB_IMOD) = 0;
    BAO1X_USB_REG(USB_IMAN) = IMAN_IE | IMAN_IP;

    BAO1X_USB_REG(USB_DCBAPLO) = (uintptr_t)dev->epcx & DCBAP_MASK;
    BAO1X_USB_REG(USB_DCBAPHI) = 0;

    BAO1X_USB_REG(USB_U3PORTPMSC) = 0;
    BAO1X_USB_REG(USB_U2PORTPMSC) = 0;

    bao1x_usb_ep0_hw_init(dev);

    dev->speed = USB_SPEED_FULL;

    BAO1X_USB_REG(USB_USBCMD) |= USBCMD_SYS_ERR_ENABLE |
                                 USBCMD_INT_ENABLE |
                                 USBCMD_RUN_STOP;
    BAO1X_USB_REG(USB_IMAN) |= IMAN_IE;

    dev->started = true;

    BAO1X_REG32(BAO1X_IRQARRAY1_BASE + IRQARRAY_EV_PENDING) =
        1U << BAO1X_USB_IRQARRAY_BIT;
    BAO1X_REG32(BAO1X_IRQARRAY1_BASE + IRQARRAY_EV_ENABLE) |=
        1U << BAO1X_USB_IRQARRAY_BIT;
    bao1x_irq_enable(BAO1X_USB_IRQ);

    /* Controller is running now; leave SE0 so the host can enumerate us. */
    _se0_release();
}

static void _dev_event(bao1x_usb_t *dev, uint8_t event)
{
    dev->dev_events |= event;
    dev->usbdev.cb(&dev->usbdev, USBDEV_EVENT_ESR);
}

static void _ep_event(bao1x_usb_ep_t *bep, uint8_t event)
{
    bep->evt |= event;
    bep->ep.dev->epcb(&bep->ep, USBDEV_EVENT_ESR);
}

static void _handle_port_event(bao1x_usb_t *dev)
{
    uint32_t portsc = BAO1X_USB_REG(USB_PORTSC);

    /* Clear the RW1C bits. */
    BAO1X_USB_REG(USB_PORTSC) = portsc;

    /* The controller is configured for full speed only. */
    dev->speed = USB_SPEED_FULL;

    if ((portsc & PORTSC_PRC) && !(portsc & PORTSC_PR)) {
        _dev_event(dev, DEV_EVT_RESET);
    }
    if (portsc & PORTSC_CSC) {
        if (portsc & PORTSC_CCS) {
            _dev_event(dev, DEV_EVT_CONNECT);
        }
        else {
            dev->have_setup = false;
            dev->setup_pending = false;
            _dev_event(dev, DEV_EVT_DISCONNECT);
        }
    }

    BAO1X_USB_REG(USB_EVENTCONFIG) |= EVENTCONFIG_SETUP_ENABLE;
}

static void _handle_transfer_event(bao1x_usb_t *dev, trb_t *event)
{
    uint32_t dw3 = event->dw3;
    unsigned pei = (dw3 >> 16) & 0x1f;
    unsigned comp = (event->dw2 >> 24) & 0xff;
    trb_t *done = (trb_t *)(uintptr_t)event->dplo;
    bao1x_usb_ep_t *bep;

    if (!_trb_ptr_valid(done)) {
        return;
    }

    unsigned type = (done->dw3 >> 10) & 0x3f;
    bool dir = (done->dw3 >> 16) & 1;

    if (pei <= 1) {
        bep = &dev->eps[0][dir ? USB_EP_DIR_OUT : USB_EP_DIR_IN];
        bep->evt_is_status = (type == TRB_TYPE_STATUS);
    }
    else if ((pei >> 1) < USBDEV_NUM_ENDPOINTS) {
        bep = &dev->eps[pei >> 1][(pei & 1) ? USB_EP_DIR_OUT : USB_EP_DIR_IN];
        bep->evt_is_status = false;
    }
    else {
        return;
    }

    if (pei >= 2) {
        bep->tx_pending = false;
    }
    bep->evt_type = type;
    bep->evt_residual = event->dw2 & 0x1ffff;
    bep->evt = EP_EVT_XFER;
    if (comp == COMP_HALTED || comp == COMP_HALTED_LEN_INVALID ||
        comp == COMP_PROTOCOL_STALL || comp == COMP_STOPPED ||
        comp == COMP_STOPPED_LEN_INVALID || comp == COMP_DISABLED) {
        bep->stalled = true;
    }
    else {
        bep->stalled = false;
    }

    dev->usbdev.epcb(&bep->ep, USBDEV_EVENT_ESR);
}

static void _process_events(bao1x_usb_t *dev)
{
    while (true) {
        trb_t *event = (trb_t *)(UDC_MEM_BASE + UDC_EVENT_OFFSET +
                                 dev->evt_idx * sizeof(trb_t));

        _barrier();
        uint32_t dw3 = event->dw3;
        bool cycle = dw3 & 1;

        if (cycle != dev->evt_ccs) {
            break;
        }

        switch ((dw3 >> 10) & 0x3f) {
        case TRB_TYPE_SETUP_EVENT:
            memcpy(dev->setup, &event->dplo, 8);
            dev->setup_tag = (dw3 >> 21) & 0x3;
            dev->setup_pending = true;
            dev->have_setup = true;
            dev->setup_read = (dev->setup[0] & 0x80) != 0;
            dev->setup_len = (uint16_t)dev->setup[6] |
                             ((uint16_t)dev->setup[7] << 8);
            dev->rx_len = 0;
            dev->tx_len = 0;
            dev->status_set_addr = false;
            _ep_event(&dev->eps[0][USB_EP_DIR_OUT], EP_EVT_XFER);
            break;
        case TRB_TYPE_XFER_EVENT:
            _handle_transfer_event(dev, event);
            break;
        case TRB_TYPE_PORT_EVENT:
            _handle_port_event(dev);
            break;
        default:
            break;
        }

        if (++dev->evt_idx >= EVENT_RING_TRBS) {
            dev->evt_idx = 0;
            dev->evt_ccs = !dev->evt_ccs;
        }
    }

    uintptr_t erdp = UDC_MEM_BASE + UDC_EVENT_OFFSET +
                     dev->evt_idx * sizeof(trb_t);
    BAO1X_USB_REG(USB_ERDPHI) = 0;
    BAO1X_USB_REG(USB_ERDPLO) = (erdp & ~0xfUL) | ERDP_EHB;
    _barrier();
}

void bao1x_usb_isr(void)
{
    bao1x_usb_t *dev = &_usbdev;

    if (!dev->started) {
        return;
    }

    uint32_t pending = BAO1X_REG32(BAO1X_IRQARRAY1_BASE + IRQARRAY_EV_PENDING);
    if (pending & (1U << BAO1X_USB_IRQARRAY_BIT)) {
        BAO1X_REG32(BAO1X_IRQARRAY1_BASE + IRQARRAY_EV_PENDING) =
            1U << BAO1X_USB_IRQARRAY_BIT;
    }

    uint32_t status = BAO1X_USB_REG(USB_USBSTS);
    if (status & USBSTS_SYSTEM_ERR) {
        BAO1X_USB_REG(USB_USBSTS) = USBSTS_SYSTEM_ERR;
    }
    if (status & USBSTS_EINT) {
        /* Clear the aggregate flag first, then the interrupter (see the UDC
         * programming guide). */
        BAO1X_USB_REG(USB_USBSTS) = USBSTS_EINT;
        BAO1X_USB_REG(USB_IMAN) |= IMAN_IP;
        _process_events(dev);
    }
}
