/*
 * SPDX-FileCopyrightText: 2026 RIOT contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_bao1x
 * @{
 *
 * @file
 * @brief       Endpoint programming for the Baochip-1x USB device controller
 *
 * @}
 */

#include <string.h>

#include "irq.h"
#include "usbdev_bao1x.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void bao1x_usb_ep0_queue_data(bao1x_usb_t *dev, bool in, uint8_t *buf,
                              size_t len)
{
    bool pcs;
    trb_t *trb = bao1x_usb_ring_get(&dev->ep0_ring, &pcs);

    trb->dplo = (uintptr_t)buf;
    trb->dphi = 0;
    trb->dw2 = ((uint32_t)len & 0x1ffff) | (1U << 18);
    trb->dw3 = (pcs ? 1U : 0U) |
               (1U << 2) |                  /* interrupt on short packet */
               (1U << 5) |                  /* interrupt on completion */
               ((uint32_t)TRB_TYPE_DATA << 10) |
               ((in ? 0U : 1U) << 16) |     /* dir: 1 = host -> device */
               ((uint32_t)dev->setup_tag << 17);
    bao1x_usb_doorbell(0);
}

void bao1x_usb_ep0_queue_status(bao1x_usb_t *dev, bool in, bool stall,
                                bool set_addr)
{
    bool pcs;
    trb_t *trb = bao1x_usb_ring_get(&dev->ep0_ring, &pcs);

    trb->dplo = 0;
    trb->dphi = 0;
    trb->dw2 = 0;
    trb->dw3 = (pcs ? 1U : 0U) |
               (1U << 5) |
               ((uint32_t)TRB_TYPE_STATUS << 10) |
               ((in ? 0U : 1U) << 16) |
               ((uint32_t)dev->setup_tag << 17) |
               ((stall ? 1U : 0U) << 19) |
               ((set_addr ? 1U : 0U) << 20);
    bao1x_usb_doorbell(0);
}

void bao1x_usb_ep_disable(bao1x_usb_t *dev, bao1x_usb_ep_t *bep)
{
    unsigned pei = _pei(bep->ep.num, bep->ep.dir);
    uint32_t mask = 1U << pei;

    if (!bep->configured) {
        return;
    }

    if (BAO1X_USB_REG(USB_EPRUNNING) & mask) {
        bao1x_usb_cmd(CMD_STOP_EP, mask, 0);
        for (unsigned i = 0; i < 1000000U; i++) {
            if (!(BAO1X_USB_REG(USB_EPRUNNING) & mask)) {
                break;
            }
        }
    }

    memset(_epcx(dev, pei), 0, sizeof(epcx_t));
    BAO1X_USB_REG(USB_EPENABLE) = mask;
    bep->configured = false;
}

void bao1x_usb_ep_enable(bao1x_usb_t *dev, bao1x_usb_ep_t *bep)
{
    unsigned pei = _pei(bep->ep.num, bep->ep.dir);
    uint32_t mask = 1U << pei;
    epcx_t *ctx = _epcx(dev, pei);
    uint32_t type;

    switch (bep->ep.type) {
    case USB_EP_TYPE_BULK:
        type = 6;
        break;
    case USB_EP_TYPE_INTERRUPT:
        type = 7;
        break;
    default:
        type = 6;
        break;
    }

    memset(ctx, 0, sizeof(epcx_t));
    ctx->dw0 = ((uint32_t)bep->ep.num << 3);
    ctx->dw1 = ((uint32_t)bep->ep.len << 16) | (15U << 8) | (type << 3);
    ctx->dw2 = ((uintptr_t)bep->ring.trbs & ~0xfUL) | (bep->ring.pcs ? 1U : 0U);
    ctx->dw3 = 0;

    if (bao1x_usb_cmd(CMD_CONFIG_EP, mask, 0) < 0) {
        return;
    }

    unsigned i;
    for (i = 0; i < 1000000U; i++) {
        if (BAO1X_USB_REG(USB_EPENABLE) & mask) {
            break;
        }
    }
    if (i == 1000000U) {
        return;
    }

    bep->configured = true;
    bep->stalled = false;
}

void bao1x_usb_ep_set_halt(bao1x_usb_t *dev, bao1x_usb_ep_t *bep, bool enable)
{
    unsigned pei = _pei(bep->ep.num, bep->ep.dir);
    uint32_t mask = 1U << pei;

    if (bep->ep.num == 0) {
        return;
    }

    unsigned state = irq_disable();

    if (enable) {
        if (!bep->stalled) {
            bao1x_usb_cmd(CMD_SET_HALT, mask, 0);
            bep->stalled = true;
        }
    }
    else {
        /* Reinitialize the transfer ring, then follow the sequence from the
         * programming guide: Reset Seqnum, Clear Halt, Set TR DQPTR. */
        bao1x_usb_ring_init(&bep->ring,
                            (void *)(UDC_MEM_BASE + UDC_EP_RINGS_OFFSET +
                                     (pei - 2) * EP_RING_SIZE),
                            EP_RING_TRBS);
        epcx_t *ctx = _epcx(dev, pei);
        ctx->dw2 = ((uintptr_t)bep->ring.trbs & ~0xfUL) |
                   (bep->ring.pcs ? 1U : 0U);
        ctx->dw3 = 0;

        bao1x_usb_cmd(CMD_RESET_SEQNUM, mask, 0);
        bao1x_usb_cmd(CMD_CLEAR_HALT, mask, 0);
        bao1x_usb_cmd(CMD_SET_TR_DQPTR, mask, 0);

        for (unsigned i = 0; i < 1000000U; i++) {
            if (BAO1X_USB_REG(USB_EPRUNNING) & mask) {
                break;
            }
        }
        bep->stalled = false;
        bao1x_usb_doorbell(pei);
    }

    irq_restore(state);
}
