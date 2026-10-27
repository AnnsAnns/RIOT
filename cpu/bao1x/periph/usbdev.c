/*
 * SPDX-FileCopyrightText: 2026 RIOT contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_bao1x
 * @{
 *
 * @file
 * @brief       RIOT usbdev API glue for the Baochip-1x USB device controller
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "irq.h"
#include "periph/usbdev.h"
#include "usbdev_bao1x.h"

#define ENABLE_DEBUG 0
#include "debug.h"

bao1x_usb_t _usbdev;

static void _ep0_xmit(bao1x_usb_t *dev, usbdev_ep_t *ep, uint8_t *buf,
                      size_t len)
{
    bao1x_usb_ep_t *bep = _bep(ep);

    if (ep->dir == USB_EP_DIR_IN) {
        if (dev->have_setup && dev->setup_read &&
            dev->tx_len < dev->setup_len && len > 0) {
            if (len > MAX_EP0_PACKET) {
                len = MAX_EP0_PACKET;
            }
            memcpy(bep->dma, buf, len);
            bep->buf = buf;
            bep->len = len;
            dev->tx_len += len;
            bao1x_usb_ep0_queue_data(dev, true, bep->dma, len);
        }
        else {
            bool set_addr = dev->status_set_addr;
            dev->status_set_addr = false;
            bao1x_usb_ep0_queue_status(dev, true, false, set_addr);
        }
    }
    else {
        if (!dev->have_setup) {
            /* Re-arm the setup buffer.  Setup packets are delivered through
             * setup events, not through the OUT transfer ring. */
            bep->buf = buf;
            bep->len = len;
            return;
        }

        if (!dev->setup_read && dev->setup_len > 0 &&
            dev->rx_len < dev->setup_len) {
            size_t chunk = dev->setup_len - dev->rx_len;
            if (chunk > len) {
                chunk = len;
            }
            if (chunk > MAX_EP0_PACKET) {
                chunk = MAX_EP0_PACKET;
            }
            bep->buf = buf;
            bep->len = chunk;
            dev->rx_len += chunk;
            bao1x_usb_ep0_queue_data(dev, false, bep->dma, chunk);
        }
        else {
            bep->buf = buf;
            bep->len = 0;
            bao1x_usb_ep0_queue_status(dev, false, false, false);
        }
    }
}

static void _usbdev_ep0_stall(usbdev_t *usbdev)
{
    bao1x_usb_t *dev = _dev(usbdev);

    unsigned state = irq_disable();

    dev->have_setup = false;
    dev->setup_pending = false;
    bao1x_usb_ep0_queue_status(dev, true, true, false);
    irq_restore(state);
}

static int _usbdev_get(usbdev_t *usbdev, usbopt_t opt, void *value,
                       size_t max_len)
{
    bao1x_usb_t *dev = _dev(usbdev);

    switch (opt) {
    case USBOPT_MAX_VERSION:
        if (max_len < sizeof(usb_version_t)) {
            return -EOVERFLOW;
        }
        *(usb_version_t *)value = USB_VERSION_20;
        return sizeof(usb_version_t);
    case USBOPT_MAX_SPEED:
    case USBOPT_ENUMERATED_SPEED:
        if (max_len < sizeof(usb_speed_t)) {
            return -EOVERFLOW;
        }
        *(usb_speed_t *)value = dev->speed;
        return sizeof(usb_speed_t);
    case USBOPT_CONNECTED:
        return -ENOTSUP;
    default:
        return -ENOTSUP;
    }
}

static int _usbdev_set(usbdev_t *usbdev, usbopt_t opt, const void *value,
                       size_t value_len)
{
    bao1x_usb_t *dev = _dev(usbdev);

    if (value_len == 0) {
        return -EINVAL;
    }

    switch (opt) {
    case USBOPT_ADDRESS: {
        uint8_t addr = *(const uint8_t *)value;

        /* With USBDEV_CPU_SET_ADDR_AFTER_STATUS == 0 this is called right
         * after the SET_ADDRESS setup packet, before the status stage. */
        if (dev->started && dev->have_setup &&
            dev->setup[0] == 0x00 && dev->setup[1] == 0x05) {
            if (bao1x_usb_cmd(CMD_SET_ADDRESS, addr, 0) == 0) {
                dev->status_set_addr = true;
            }
        }
        return value_len;
    }
    case USBOPT_ATTACH:
        if (*(const usbopt_enable_t *)value == USBOPT_ENABLE) {
            bao1x_usb_attach(true);
        }
        else {
            bao1x_usb_attach(false);
        }
        return value_len;
    default:
        return -ENOTSUP;
    }
}

static int _usbdev_ep_get(usbdev_ep_t *ep, usbopt_ep_t opt, void *value,
                          size_t max_len)
{
    bao1x_usb_ep_t *bep = _bep(ep);

    switch (opt) {
    case USBOPT_EP_AVAILABLE:
        if (max_len < sizeof(size_t)) {
            return -EOVERFLOW;
        }
        *(size_t *)value = bep->avail;
        return sizeof(size_t);
    default:
        return -ENOTSUP;
    }
}

static void _usbdev_ep_stall(usbdev_ep_t *ep, bool enable)
{
    bao1x_usb_ep_set_halt(_dev(ep->dev), _bep(ep), enable);
}

static void _usbdev_ep_init(usbdev_ep_t *ep)
{
    bao1x_usb_t *dev = _dev(ep->dev);

    if (ep->num == 0) {
        bao1x_usb_ep0_hw_init(dev);
        return;
    }

    bao1x_usb_ep_t *bep = _bep(ep);
    unsigned pei = _pei(ep->num, ep->dir);

    bao1x_usb_ring_init(&bep->ring,
                        (void *)(UDC_MEM_BASE + UDC_EP_RINGS_OFFSET +
                                 (pei - 2) * EP_RING_SIZE),
                        EP_RING_TRBS);
    bep->configured = false;
    bep->stalled = false;
    bep->avail = 0;
}

static int _usbdev_ep_set(usbdev_ep_t *ep, usbopt_ep_t opt,
                          const void *value, size_t value_len)
{
    bao1x_usb_t *dev = _dev(ep->dev);
    bao1x_usb_ep_t *bep = _bep(ep);

    switch (opt) {
    case USBOPT_EP_ENABLE:
        if (*(const usbopt_enable_t *)value == USBOPT_ENABLE) {
            if (ep->num != 0 && !bep->configured) {
                unsigned state = irq_disable();
                _usbdev_ep_init(ep);
                bao1x_usb_ep_enable(dev, bep);
                if (!bep->configured) {
                    irq_restore(state);
                    return -EIO;
                }
                irq_restore(state);
            }
        }
        else if (ep->num != 0) {
            unsigned state = irq_disable();
            bao1x_usb_ep_disable(dev, bep);
            irq_restore(state);
        }
        return value_len;
    case USBOPT_EP_STALL:
        _usbdev_ep_stall(ep, *(const usbopt_enable_t *)value == USBOPT_ENABLE);
        return value_len;
    default:
        return -ENOTSUP;
    }
}

static int _usbdev_ep_xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len)
{
    bao1x_usb_t *dev = _dev(ep->dev);
    bao1x_usb_ep_t *bep = _bep(ep);

    if (len > MAX_BOUNCE_LEN) {
        return -EMSGSIZE;
    }

    if (buf == NULL && len != 0) {
        return -EINVAL;
    }

    unsigned state = irq_disable();

    if (ep->num == 0) {
        _ep0_xmit(dev, ep, buf, len);
        irq_restore(state);
        return 0;
    }

    if (len > ep->len) {
        len = ep->len;
    }

    uint32_t mask = 1U << _pei(ep->num, ep->dir);
    if (!bep->configured || !(BAO1X_USB_REG(USB_EPRUNNING) & mask)) {
        irq_restore(state);
        return -EIO;
    }

    bep->buf = buf;
    bep->len = len;

    if (ep->dir == USB_EP_DIR_IN && len != 0) {
        memcpy(bep->dma, buf, len);
    }

    bool pcs;
    trb_t *trb = bao1x_usb_ring_get(&bep->ring, &pcs);
    trb->dplo = (uintptr_t)bep->dma;
    trb->dphi = 0;
    trb->dw2 = ((uint32_t)len & 0x1ffff) | (1U << 18);
    trb->dw3 = (pcs ? 1U : 0U) |
               (1U << 5) |
               ((uint32_t)TRB_TYPE_NORMAL << 10);

    bep->tx_pending = true;
    bao1x_usb_doorbell(_pei(ep->num, ep->dir));
    if (ep->dir == USB_EP_DIR_OUT) {
        bao1x_usb_kick_start();
    }

    irq_restore(state);
    return 0;
}

static void _usbdev_ep_esr(usbdev_ep_t *ep)
{
    bao1x_usb_ep_t *bep = _bep(ep);
    bao1x_usb_t *dev = _dev(ep->dev);
    bool completed = false;
    bool stalled;
    bool is_status;

    unsigned state = irq_disable();

    bool setup = (ep->num == 0 && ep->dir == USB_EP_DIR_OUT &&
                  dev->setup_pending);

    if (setup) {
        dev->setup_pending = false;
        size_t copy = sizeof(dev->setup);
        if (bep->buf == NULL) {
            copy = 0;
        }
        else if (copy > bep->len) {
            copy = bep->len;
        }
        if (copy) {
            memcpy(bep->buf, dev->setup, copy);
        }
        bep->avail = copy;
        bep->evt = EP_EVT_NONE;
        completed = true;
        is_status = false;
        stalled = false;
    }
    else if (bep->evt) {
        bep->evt = EP_EVT_NONE;
        completed = true;
        is_status = bep->evt_is_status;
        stalled = bep->stalled;

        if ((ep->num != 0 || bep->evt_type == TRB_TYPE_DATA) &&
            ep->dir == USB_EP_DIR_OUT && bep->len >= bep->evt_residual) {
            size_t actual = bep->len - bep->evt_residual;
            bep->avail = actual;
            if (bep->buf != NULL && actual) {
                memcpy(bep->buf, bep->dma, actual);
            }
        }
        else if (ep->dir == USB_EP_DIR_IN) {
            bep->avail = 0;
        }

        if (is_status && ep->num == 0) {
            if (!dev->have_setup) {
                /* Stall status stage or a late completion. */
                completed = false;
            }
            else {
                dev->have_setup = false;
                dev->rx_len = 0;
                dev->tx_len = 0;
                dev->status_set_addr = false;
            }
        }
    }

    irq_restore(state);

    if (completed) {
        bep->ep.dev->epcb(&bep->ep,
                          stalled ? USBDEV_EVENT_TR_STALL
                                  : USBDEV_EVENT_TR_COMPLETE);
    }
}

static void _usbdev_esr(usbdev_t *usbdev)
{
    bao1x_usb_t *dev = _dev(usbdev);
    uint8_t events;

    unsigned state = irq_disable();

    events = dev->dev_events;
    dev->dev_events = 0;
    irq_restore(state);

    if (events & DEV_EVT_RESET) {
        /* The bus reset resets the address and the endpoint state.  Drop the
         * non-control endpoints; the host reconfigures them with
         * SET_CONFIGURATION. */
        unsigned irq_state = irq_disable();
        for (unsigned num = 1; num < USBDEV_NUM_ENDPOINTS; num++) {
            bao1x_usb_ep_disable(dev, &dev->eps[num][USB_EP_DIR_OUT]);
            bao1x_usb_ep_disable(dev, &dev->eps[num][USB_EP_DIR_IN]);
        }
        BAO1X_USB_REG(USB_EVENTCONFIG) |= EVENTCONFIG_SETUP_ENABLE;
        irq_restore(irq_state);

        usbdev->cb(usbdev, USBDEV_EVENT_RESET);
    }
    if (events & DEV_EVT_CONNECT) {
        usbdev->cb(usbdev, USBDEV_EVENT_HOST_CONNECT);
    }
    if (events & DEV_EVT_DISCONNECT) {
        usbdev->cb(usbdev, USBDEV_EVENT_HOST_DISCONNECT);
    }
    if (events & DEV_EVT_SUSPEND) {
        usbdev->cb(usbdev, USBDEV_EVENT_SUSPEND);
    }
    if (events & DEV_EVT_RESUME) {
        usbdev->cb(usbdev, USBDEV_EVENT_RESUME);
    }
}

static usbdev_ep_t *_usbdev_new_ep(usbdev_t *usbdev, usb_ep_type_t type,
                                   usb_ep_dir_t dir, size_t len)
{
    bao1x_usb_t *dev = _dev(usbdev);

    if (len > MAX_BOUNCE_LEN) {
        return NULL;
    }

    if (type == USB_EP_TYPE_CONTROL) {
        bao1x_usb_ep_t *bep = &dev->eps[0][dir];
        bep->ep.dev = usbdev;
        bep->ep.num = 0;
        bep->ep.dir = dir;
        bep->ep.type = type;
        bep->ep.len = len;
        bep->dma = _bounce(0, dir);
        return &bep->ep;
    }

    unsigned num = 0;

    /* The UDC wants the two directions of a bulk/interrupt endpoint to share
     * the same logical endpoint number.  Prefer pairing with an existing
     * endpoint of the same type and the opposite direction. */
    for (unsigned i = 1; i < USBDEV_NUM_ENDPOINTS; i++) {
        bao1x_usb_ep_t *bep = &dev->eps[i][dir];
        bao1x_usb_ep_t *peer = &dev->eps[i][dir == USB_EP_DIR_IN ? USB_EP_DIR_OUT
                                                                 : USB_EP_DIR_IN];
        if (bep->ep.dev == NULL && peer->ep.dev != NULL &&
            peer->ep.type == type && type != USB_EP_TYPE_CONTROL) {
            num = i;
            break;
        }
    }

    if (num == 0) {
        for (unsigned i = 1; i < USBDEV_NUM_ENDPOINTS; i++) {
            if (dev->eps[i][USB_EP_DIR_OUT].ep.dev == NULL &&
                dev->eps[i][USB_EP_DIR_IN].ep.dev == NULL) {
                num = i;
                break;
            }
        }
    }

    if (num == 0) {
        return NULL;
    }

    bao1x_usb_ep_t *bep = &dev->eps[num][dir];
    memset(bep, 0, sizeof(*bep));
    bep->ep.dev = usbdev;
    bep->ep.num = num;
    bep->ep.dir = dir;
    bep->ep.type = type;
    bep->ep.len = len;
    bep->dma = _bounce(num, dir);
    return &bep->ep;
}

static void _usbdev_init(usbdev_t *usbdev)
{
    bao1x_usb_t *dev = _dev(usbdev);

    bao1x_usb_hw_init(dev);
}

static const usbdev_driver_t _driver = {
    .init = _usbdev_init,
    .new_ep = _usbdev_new_ep,
    .get = _usbdev_get,
    .set = _usbdev_set,
    .esr = _usbdev_esr,
    .ep0_stall = _usbdev_ep0_stall,
    .ep_init = _usbdev_ep_init,
    .ep_stall = _usbdev_ep_stall,
    .ep_get = _usbdev_ep_get,
    .ep_set = _usbdev_ep_set,
    .ep_esr = _usbdev_ep_esr,
    .xmit = _usbdev_ep_xmit,
};

void usbdev_init_lowlevel(void)
{
    memset(&_usbdev, 0, sizeof(_usbdev));
    _usbdev.usbdev.driver = &_driver;
    _usbdev.speed = USB_SPEED_FULL;

    /* Keep the port in SE0 while we initialize.  boot1 leaves the switch in
     * SE0 already, but holding it for a while guarantees that the host sees a
     * disconnect and re-enumerates this firmware. */
    bao1x_usb_preinit();
}

usbdev_t *usbdev_get_ctx(unsigned num)
{
    if (num != 0) {
        return NULL;
    }
    return &_usbdev.usbdev;
}
