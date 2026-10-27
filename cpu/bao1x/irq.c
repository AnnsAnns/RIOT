/*
 * SPDX-FileCopyrightText: 2026 RIOT contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_bao1x
 * @{
 *
 * @file
 * @brief       Baochip-1x external interrupt dispatch
 */

#include <stddef.h>

#include "irq_arch.h"
#include "bao1x_irq.h"

#define REG32(a)            (*(volatile uint32_t *)(uintptr_t)(a))

#define TIMER0_LOAD         (BAO1X_TIMER0_BASE + 0x00)
#define TIMER0_RELOAD       (BAO1X_TIMER0_BASE + 0x04)
#define TIMER0_EN           (BAO1X_TIMER0_BASE + 0x08)
#define TIMER0_EV_PENDING   (BAO1X_TIMER0_BASE + 0x18)
#define TIMER0_EV_ENABLE    (BAO1X_TIMER0_BASE + 0x1c)

static void (*timer0_cb)(void *);
static void *timer0_arg;

#ifdef MODULE_PERIPH_USBDEV
extern void bao1x_usb_isr(void);
#endif

static inline uint32_t mim_read(void)
{
    uint32_t v;

    __asm__ volatile ("csrr %0, %1" : "=r" (v) : "i" (BAO1X_CSR_MIM));
    return v;
}

static inline void mim_set(uint32_t mask)
{
    __asm__ volatile ("csrs %0, %1" : : "i" (BAO1X_CSR_MIM), "r" (mask));
}

static inline uint32_t mip_vex_read(void)
{
    uint32_t v;

    __asm__ volatile ("csrr %0, %1" : "=r" (v) : "i" (BAO1X_CSR_MIP_VEX));
    return v;
}

void bao1x_timer0_set_callback(void (*cb)(void *), void *arg)
{
    timer0_cb = cb;
    timer0_arg = arg;
}

void bao1x_timer0_start(uint32_t load)
{
    REG32(TIMER0_EN) = 0;
    REG32(TIMER0_EV_ENABLE) = 0;
    REG32(TIMER0_EV_PENDING) = 1;
    REG32(TIMER0_LOAD) = load;
    REG32(TIMER0_RELOAD) = load;
    REG32(TIMER0_EV_ENABLE) = 1;
    mim_set(1U << BAO1X_TIMER0_IRQ);
    REG32(TIMER0_EN) = 1;
}

void bao1x_timer0_stop(void)
{
    REG32(TIMER0_EN) = 0;
    REG32(TIMER0_EV_ENABLE) = 0;
}

void riscv_external_irq_handler(void)
{
    uint32_t pending = mip_vex_read();

#ifdef MODULE_PERIPH_USBDEV
    if (pending & (1U << BAO1X_USB_IRQ)) {
        bao1x_usb_isr();
    }
#endif

    if (pending & (1U << BAO1X_TIMER0_IRQ)) {
        REG32(TIMER0_EV_PENDING) = 1;
        if (timer0_cb) {
            timer0_cb(timer0_arg);
        }
    }
}

/** @} */
