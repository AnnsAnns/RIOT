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

#define TICKTIMER_EV_PENDING    (0xe001b018UL)
#define TIMER0_EV_PENDING       (0xe001c018UL)

#ifdef MODULE_PERIPH_USBDEV
extern void bao1x_usb_isr(void);
extern void bao1x_usb_timer_isr(void);
#endif
#ifdef MODULE_PERIPH_TIMER
extern void bao1x_timer_isr(void);
#endif

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

void bao1x_irq_enable(unsigned irq)
{
    mim_set(1U << irq);
}

void riscv_external_irq_handler(void)
{
    uint32_t pending = mip_vex_read();

    (void)pending;

#ifdef MODULE_PERIPH_USBDEV
    if (pending & (1U << BAO1X_USB_IRQ)) {
        bao1x_usb_isr();
    }
    if (pending & (1U << BAO1X_USB_TIMER_IRQ)) {
        REG32(TIMER0_EV_PENDING) = 1;
        bao1x_usb_timer_isr();
    }
#endif

#ifdef MODULE_PERIPH_TIMER
    if (pending & (1U << BAO1X_TICKTIMER_IRQ)) {
        REG32(TICKTIMER_EV_PENDING) = 1;
        bao1x_timer_isr();
    }
#endif
}

/** @} */
