#pragma once

#include "periph_cpu.h"

#ifdef RP2350_USE_RISCV
#  include "xh3irq.h"
#endif

static inline void rp_irq_enable(uint32_t irq_no)
{
#ifdef RP2350_USE_RISCV
    xh3irq_enable_irq(irq_no);
#else
    NVIC_EnableIRQ(irq_no);
#endif
}

static inline void rp_end_isr(void)
{
#ifdef RP2350_USE_ARM
    cortexm_isr_end();
#endif
}
