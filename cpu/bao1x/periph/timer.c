/*
 * SPDX-FileCopyrightText: 2026 RIOT contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_bao1x
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver for the Baochip-1x TickTimer
 *
 * The TickTimer is a free-running 64-bit millisecond counter with a single
 * compare alarm.  It is the only free-running, interrupt-capable timer that is
 * wired to the RISC-V core, so it provides a 1 kHz periph_timer device.
 *
 * @}
 */

#include <stdint.h>

#include "irq.h"
#include "periph/timer.h"
#include "bao1x_irq.h"

#define TICKTIMER_BASE              (0xe001b000UL)
#define TICKTIMER_TIME1             (TICKTIMER_BASE + 0x04)
#define TICKTIMER_TIME0             (TICKTIMER_BASE + 0x08)
#define TICKTIMER_TARGET1           (TICKTIMER_BASE + 0x0c)
#define TICKTIMER_TARGET0           (TICKTIMER_BASE + 0x10)
#define TICKTIMER_EV_PENDING        (TICKTIMER_BASE + 0x18)
#define TICKTIMER_EV_ENABLE         (TICKTIMER_BASE + 0x1c)

#define REG32(addr)                 (*(volatile uint32_t *)(uintptr_t)(addr))

static timer_isr_ctx_t _isr_ctx;
static bool _running;
static bool _armed;

static uint64_t _now64(void)
{
    uint32_t hi = REG32(TICKTIMER_TIME1);
    uint32_t lo = REG32(TICKTIMER_TIME0);

    return ((uint64_t)hi << 32) | lo;
}

void bao1x_timer_isr(void)
{
    REG32(TICKTIMER_EV_ENABLE) = 0;
    REG32(TICKTIMER_EV_PENDING) = 1;
    _armed = false;

    if (_isr_ctx.cb) {
        _isr_ctx.cb(_isr_ctx.arg, 0);
    }
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    if (dev >= TIMER_NUMOF || freq != TIMER_0_FREQ) {
        return -1;
    }

    _isr_ctx.cb = cb;
    _isr_ctx.arg = arg;

    unsigned state = irq_disable();

    REG32(TICKTIMER_EV_ENABLE) = 0;
    REG32(TICKTIMER_EV_PENDING) = 1;
    bao1x_irq_enable(BAO1X_TICKTIMER_IRQ);
    _running = true;
    _armed = false;

    irq_restore(state);
    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (dev >= TIMER_NUMOF || channel != 0) {
        return -1;
    }

    unsigned state = irq_disable();

    uint64_t now = _now64();
    uint64_t target = (now & ~(uint64_t)UINT32_MAX) | value;
    if (target <= now) {
        target += (uint64_t)UINT32_MAX + 1;
    }

    REG32(TICKTIMER_EV_ENABLE) = 0;
    REG32(TICKTIMER_TARGET1) = (uint32_t)(target >> 32);
    REG32(TICKTIMER_TARGET0) = (uint32_t)target;
    REG32(TICKTIMER_EV_PENDING) = 1;
    _armed = true;
    if (_running) {
        REG32(TICKTIMER_EV_ENABLE) = 1;
    }

    irq_restore(state);
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (dev >= TIMER_NUMOF || channel != 0) {
        return -1;
    }

    unsigned state = irq_disable();
    REG32(TICKTIMER_EV_ENABLE) = 0;
    _armed = false;
    irq_restore(state);
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    (void)dev;
    return REG32(TICKTIMER_TIME0);
}

void timer_start(tim_t dev)
{
    (void)dev;

    /* The TickTimer counter is free-running; starting means re-enabling the
     * alarm interrupt. */
    unsigned state = irq_disable();
    _running = true;
    if (_armed) {
        REG32(TICKTIMER_EV_ENABLE) = 1;
    }
    irq_restore(state);
}

void timer_stop(tim_t dev)
{
    (void)dev;

    unsigned state = irq_disable();
    _running = false;
    REG32(TICKTIMER_EV_ENABLE) = 0;
    irq_restore(state);
}
