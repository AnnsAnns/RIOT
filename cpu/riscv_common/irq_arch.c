/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_riscv_common
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU IRQ management for RISC-V clint/plic
 *              peripheral
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "macros/xtstr.h"
#include "cpu.h"
#include "context_frame.h"
#include "irq.h"
#include "irq_arch.h"
#include "panic.h"
#include "sched.h"
#include "plic.h"
#include "clic.h"
#include "architecture.h"

#include "vendor/riscv_csr.h"

/* Default state of mstatus register */
#define MSTATUS_DEFAULT     (MSTATUS_MPP | MSTATUS_MPIE)


#define WEAK_DEFAULT    __attribute__((weak,alias("dummy_handler")))

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {     core_panic(PANIC_GENERAL_ERROR, "DUMMY HANDLER"); }

/* rp2350 specific interrupt vector */
WEAK_DEFAULT void isr_timer0_0(void);
WEAK_DEFAULT void isr_timer0_1(void);
WEAK_DEFAULT void isr_timer0_2(void);
WEAK_DEFAULT void isr_timer0_3(void);
WEAK_DEFAULT void isr_timer1_0(void);
WEAK_DEFAULT void isr_timer1_1(void);
WEAK_DEFAULT void isr_timer1_2(void);
WEAK_DEFAULT void isr_timer1_3(void);
WEAK_DEFAULT void isr_pwm_wrap_0(void);
WEAK_DEFAULT void isr_pwm_wrap_1(void);
WEAK_DEFAULT void isr_dma_0(void);
WEAK_DEFAULT void isr_dma_1(void);
WEAK_DEFAULT void isr_dma_2(void);
WEAK_DEFAULT void isr_dma_3(void);
WEAK_DEFAULT void isr_usbctrl(void);
WEAK_DEFAULT void isr_pio0_0(void);
WEAK_DEFAULT void isr_pio0_1(void);
WEAK_DEFAULT void isr_pio1_0(void);
WEAK_DEFAULT void isr_pio1_1(void);
WEAK_DEFAULT void isr_pio2_0(void);
WEAK_DEFAULT void isr_pio2_1(void);
WEAK_DEFAULT void isr_io_bank0(void);
WEAK_DEFAULT void isr_io_bank0_ns(void);
WEAK_DEFAULT void isr_io_qspi(void);
WEAK_DEFAULT void isr_io_qspi_ns(void);
WEAK_DEFAULT void isr_sio_fifo(void);
WEAK_DEFAULT void isr_sio_bell(void);
WEAK_DEFAULT void isr_sio_fifo_ns(void);
WEAK_DEFAULT void isr_sio_bell_ns(void);
WEAK_DEFAULT void isr_sio_mtimecmp(void);
WEAK_DEFAULT void isr_clocks(void);
WEAK_DEFAULT void isr_spi0(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_adc_fifo(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_otp(void);
WEAK_DEFAULT void isr_trng(void);
WEAK_DEFAULT void isr_proc0_cti(void);
WEAK_DEFAULT void isr_proc1_cti(void);
WEAK_DEFAULT void isr_pll_sys(void);
WEAK_DEFAULT void isr_pll_usb(void);
WEAK_DEFAULT void isr_powman_pow(void);
WEAK_DEFAULT void isr_powman_timer(void);
WEAK_DEFAULT void isr_spareirq_0(void);
WEAK_DEFAULT void isr_spareirq_1(void);
WEAK_DEFAULT void isr_spareirq_2(void);
WEAK_DEFAULT void isr_spareirq_3(void);
WEAK_DEFAULT void isr_spareirq_4(void);
void isr_spareirq_5(void) {
    printf("Hello from spareirq 5 :D\n");
}

/** CPU specific interrupt vector table
 * @see 3.2 Interrupts and IRQn_Type in RP2350.h
 */
const void* vector_cpu[CPU_IRQ_NUMOF] = {
    (void*)isr_timer0_0,     /* 0 TIMER0_IRQ_0 */
    (void*)isr_timer0_1,     /* 1 TIMER0_IRQ_1 */
    (void*)isr_timer0_2,     /* 2 TIMER0_IRQ_2 */
    (void*)isr_timer0_3,     /* 3 TIMER0_IRQ_3 */
    (void*)isr_timer1_0,     /* 4 TIMER1_IRQ_0 */
    (void*)isr_timer1_1,     /* 5 TIMER1_IRQ_1 */
    (void*)isr_timer1_2,     /* 6 TIMER1_IRQ_2 */
    (void*)isr_timer1_3,     /* 7 TIMER1_IRQ_3 */
    (void*)isr_pwm_wrap_0,   /* 8 PWM_IRQ_WRAP_0 */
    (void*)isr_pwm_wrap_1,   /* 9 PWM_IRQ_WRAP_1 */
    (void*)isr_dma_0,        /* 10 DMA_IRQ_0 */
    (void*)isr_dma_1,        /* 11 DMA_IRQ_1 */
    (void*)isr_dma_2,        /* 12 DMA_IRQ_2 */
    (void*)isr_dma_3,        /* 13 DMA_IRQ_3 */
    (void*)isr_usbctrl,      /* 14 USBCTRL_IRQ */
    (void*)isr_pio0_0,       /* 15 PIO0_IRQ_0 */
    (void*)isr_pio0_1,       /* 16 PIO0_IRQ_1 */
    (void*)isr_pio1_0,       /* 17 PIO1_IRQ_0 */
    (void*)isr_pio1_1,       /* 18 PIO1_IRQ_1 */
    (void*)isr_pio2_0,       /* 19 PIO2_IRQ_0 */
    (void*)isr_pio2_1,       /* 20 PIO2_IRQ_1 */
    (void*)isr_io_bank0,     /* 21 IO_IRQ_BANK0 */
    (void*)isr_io_bank0_ns,  /* 22 IO_IRQ_BANK0_NS */
    (void*)isr_io_qspi,      /* 23 IO_IRQ_QSPI */
    (void*)isr_io_qspi_ns,   /* 24 IO_IRQ_QSPI_NS */
    (void*)isr_sio_fifo,     /* 25 SIO_IRQ_FIFO */
    (void*)isr_sio_bell,     /* 26 SIO_IRQ_BELL */
    (void*)isr_sio_fifo_ns,  /* 27 SIO_IRQ_FIFO_NS */
    (void*)isr_sio_bell_ns,  /* 28 SIO_IRQ_BELL_NS */
    (void*)isr_sio_mtimecmp, /* 29 SIO_IRQ_MTIMECMP */
    (void*)isr_clocks,       /* 30 CLOCKS_IRQ */
    (void*)isr_spi0,         /* 31 SPI0_IRQ */
    (void*)isr_spi1,         /* 32 SPI1_IRQ */
    (void*)isr_uart0,        /* 33 UART0_IRQ */
    (void*)isr_uart1,        /* 34 UART1_IRQ */
    (void*)isr_adc_fifo,     /* 35 ADC_IRQ_FIFO */
    (void*)isr_i2c0,         /* 36 I2C0_IRQ */
    (void*)isr_i2c1,         /* 37 I2C1_IRQ */
    (void*)isr_otp,          /* 38 OTP_IRQ */
    (void*)isr_trng,         /* 39 TRNG_IRQ */
    (void*)isr_proc0_cti,    /* 40 PROC0_IRQ_CTI */
    (void*)isr_proc1_cti,    /* 41 PROC1_IRQ_CTI */
    (void*)isr_pll_sys,      /* 42 PLL_SYS_IRQ */
    (void*)isr_pll_usb,      /* 43 PLL_USB_IRQ */
    (void*)isr_powman_pow,   /* 44 POWMAN_IRQ_POW */
    (void*)isr_powman_timer, /* 45 POWMAN_IRQ_TIMER */
    (void*)isr_spareirq_0,   /* 46 SPAREIRQ_IRQ_0 */
    (void*)isr_spareirq_1,   /* 47 SPAREIRQ_IRQ_1 */
    (void*)isr_spareirq_2,   /* 48 SPAREIRQ_IRQ_2 */
    (void*)isr_spareirq_3,   /* 49 SPAREIRQ_IRQ_3 */
    (void*)isr_spareirq_4,   /* 50 SPAREIRQ_IRQ_4 */
    (void*)isr_spareirq_5,   /* 51 SPAREIRQ_IRQ_5 */
};

volatile int riscv_in_isr = 0;

/**
 * @brief   ISR trap vector
 */
static void trap_entry(void);

/**
 * @brief   Timer ISR
 */
void timer_isr(void);

void riscv_irq_init(void)
{
    /* Setup trap handler function */
    if (IS_ACTIVE(MODULE_PERIPH_CLIC)) {
        /* Signal CLIC usage to the core */
        write_csr(mtvec, (uintptr_t)&trap_entry | 0x03);
    }
    else {
        write_csr(mtvec, (uintptr_t)&trap_entry);
    }

    /* Clear all interrupt enables */
    write_csr(mie, 0);

    /* Initial PLIC external interrupt controller */
    if (IS_ACTIVE(MODULE_PERIPH_PLIC)) {
        plic_init();
    }
    if (IS_ACTIVE(MODULE_PERIPH_CLIC)) {
        clic_init();
    }

    /* Enable external interrupts */
    set_csr(mie, MIP_MEIP);

    /*  Set default state of mstatus */
    set_csr(mstatus, MSTATUS_DEFAULT);

    irq_enable();
}

/**
 * @brief Global trap and interrupt handler
 */
__attribute((used))
static void handle_trap(uword_t mcause)
{
    /*  Tell RIOT to set sched_context_switch_request instead of
     *  calling thread_yield(). */
    riscv_in_isr = 1;

    uword_t trap = mcause & CPU_CSR_MCAUSE_CAUSE_MSK;

    // Get MEINEXT at 0xbe4 which is the next highest interrupt to handle (Bit 2-10)
    uint32_t meinext = (read_csr(0xbe4) >> 2) & 0x1ff;

    printf("Trap: mcause=0x%" PRIx32 " mepc=0x%lx mtval=0x%lx meinext=0x%lx\n",
           (uint32_t)mcause, read_csr(mepc), read_csr(mtval), meinext);

    if (meinext > 0) {
        /* Call the interrupt handler directly from the vector table */
        void (*isr)(void) = (void (*)(void))vector_cpu[meinext];
        printf("Calling isr %p for irq %ld\n", isr, meinext);
        isr();
        riscv_in_isr = 0;
    } else if ((mcause & ~MCAUSE_INT) <= 0xb) {
        const char* error_messages[] = {
            "Instruction alignment: Does not occur on RP2350, because 16-bit compressed instructions are implemented, and it is impossible to jump to a byte-aligned address.",
            "Instruction fetch fault: Attempted to fetch from an address that does not support instruction fetch (like APB/AHB peripherals on RP2350), or lacks PMP execute permission, or is forbidden by ACCESSCTRL, or returned a fault from the memory device itself.",
            "Illegal instruction: Encountered an instruction that was not a valid RISC-V opcode implemented by this processor, or attempted to access a nonexistent CSR, or attempted to execute a privileged instruction or access a privileged CSR without sufficient privilege.",
            "Breakpoint: An ebreak or c.ebreak instruction was executed, and no external debug host caught it (DCSR.EBREAKM or DCSR.EBREAKU was not set).",
            "Load alignment: Attempted to load from an address that was not a multiple of access size.",
            "Load fault: Attempted to load from an address that does not exist, or lacks PMP read permissions, or is forbidden by ACCESSCTRL, or returned a fault from a peripheral.",
            "Store/AMO alignment: Attempted to write to an address that was not a multiple of access size.",
            "Store/AMO fault: Attempted to write to an address that does not exist, or lacks PMP write permissions, or is forbidden by ACCESSCTRL, or returned a fault from a peripheral. Also raised when attempting an AMO on an address that does not support AHB5 exclusives.",
            "An ecall instruction was executed in U-mode.",
            NULL, /* 0x9 - not defined */
            NULL, /* 0xa - not defined */
            "An ecall instruction was executed in M-mode."
        };

        uword_t cause_code = mcause & ~MCAUSE_INT;
        if (cause_code <= 0xb && error_messages[cause_code] != NULL) {
            printf("Error 0x%lx: %s\n", cause_code, error_messages[cause_code]);
        }
    }

    /* Check for INT or TRAP */
    if ((mcause & MCAUSE_INT) == MCAUSE_INT) {
        /* Cause is an interrupt - determine type */
        switch (mcause & MCAUSE_CAUSE) {

#ifdef MODULE_PERIPH_CORETIMER
        case IRQ_M_TIMER:
            /* Handle timer interrupt */
            timer_isr();
            break;
#endif
        case IRQ_M_EXT:
            /* Handle external interrupt */
            if (IS_ACTIVE(MODULE_PERIPH_PLIC)) {
                plic_isr_handler();
            }
            break;

        default:
            if (IS_ACTIVE(MODULE_PERIPH_CLIC)) {
                clic_isr_handler(trap);
            }
            else {
                /* Unknown interrupt */
                core_panic(PANIC_GENERAL_ERROR, "Unhandled interrupt");
            }
            break;
        }
    }
    else {
        switch (trap) {
        case CAUSE_USER_ECALL:      /* ECALL from user mode */
        case CAUSE_MACHINE_ECALL:   /* ECALL from machine mode */
        {
            /* TODO: get the ecall arguments */
            sched_context_switch_request = 1;
            /* Increment the return program counter past the ecall
             * instruction */
            uword_t return_pc = read_csr(mepc);
            write_csr(mepc, return_pc + 4);
            break;
        }
#ifdef MODULE_PERIPH_PMP
        case CAUSE_FAULT_FETCH:
            core_panic(PANIC_MEM_MANAGE, "MEM MANAGE HANDLER (fetch)");
        case CAUSE_FAULT_LOAD:
            core_panic(PANIC_MEM_MANAGE, "MEM MANAGE HANDLER (load)");
        case CAUSE_FAULT_STORE:
            core_panic(PANIC_MEM_MANAGE, "MEM MANAGE HANDLER (store)");
#endif
        default:
#ifdef DEVELHELP
            printf("Unhandled trap:\n");
            printf("  mcause: 0x%" PRIx32 "\n", trap);
            printf("  mepc:   0x%lx\n", read_csr(mepc));
            printf("  mtval:  0x%lx\n", read_csr(mtval));
#endif
            /* Unknown trap */
            core_panic(PANIC_GENERAL_ERROR, "Unhandled trap");
        }
    }
    /* ISR done - no more changes to thread states */
    riscv_in_isr = 0;
}

/* Marking this as interrupt to ensure an mret at the end, provided by the
 * compiler. Aligned to 64-byte boundary as per RISC-V spec and required by some
 * of the supported platforms (gd32)*/
__attribute((aligned(64)))
static void __attribute__((interrupt)) trap_entry(void)
{
    __asm__ volatile (
        "addi sp, sp, -"XTSTR (CONTEXT_FRAME_SIZE)"          \n"

        /* Save caller-saved registers */
        "sw ra, "XTSTR (ra_OFFSET)"(sp)                      \n"
        "sw t0, "XTSTR (t0_OFFSET)"(sp)                      \n"
        "sw t1, "XTSTR (t1_OFFSET)"(sp)                      \n"
        "sw t2, "XTSTR (t2_OFFSET)"(sp)                      \n"
        "sw t3, "XTSTR (t3_OFFSET)"(sp)                      \n"
        "sw t4, "XTSTR (t4_OFFSET)"(sp)                      \n"
        "sw t5, "XTSTR (t5_OFFSET)"(sp)                      \n"
        "sw t6, "XTSTR (t6_OFFSET)"(sp)                      \n"
        "sw a0, "XTSTR (a0_OFFSET)"(sp)                      \n"
        "sw a1, "XTSTR (a1_OFFSET)"(sp)                      \n"
        "sw a2, "XTSTR (a2_OFFSET)"(sp)                      \n"
        "sw a3, "XTSTR (a3_OFFSET)"(sp)                      \n"
        "sw a4, "XTSTR (a4_OFFSET)"(sp)                      \n"
        "sw a5, "XTSTR (a5_OFFSET)"(sp)                      \n"
        "sw a6, "XTSTR (a6_OFFSET)"(sp)                      \n"
        "sw a7, "XTSTR (a7_OFFSET)"(sp)                      \n"

        /* Save s0 and s1 extra for the active thread and the stack ptr */
        "sw s0, "XTSTR (s0_OFFSET)"(sp)                      \n"
        "sw s1, "XTSTR (s1_OFFSET)"(sp)                      \n"

        /* Save the user stack ptr */
        "mv s0, sp                                          \n"
        /* Load exception stack ptr */
        "la sp, _sp                                         \n"

        /* Get the interrupt cause */
        "csrr a0, mcause                                    \n"

        /* Call trap handler, a0 contains mcause before, and the return value after
         * the call */
        "call handle_trap                                   \n"

        /* Load the sched_context_switch_request */
        "lw a0, sched_context_switch_request                \n"

        /* And skip the context switch if not requested */
        "beqz a0, no_sched                                  \n"

        /*  Get the previous active thread (could be NULL) */
        "lw s1, sched_active_thread                         \n"

        /* Run the scheduler */
        "call sched_run                                     \n"

        "no_sched:                                          \n"
        /* Restore the thread stack pointer and check if a new thread must be
         * scheduled */
        "mv sp, s0                                          \n"

        /* No context switch required, shortcut to restore. a0 contains the return
         * value of sched_run, or the sched_context_switch_request if the sched_run
         * was skipped */
        "beqz a0, no_switch                                 \n"

        /* Skips the rest of the save if no active thread */
        "beqz s1, null_thread                               \n"

        /* Store s2-s11 */
        "sw s2, "XTSTR (s2_OFFSET)"(sp)                      \n"
        "sw s3, "XTSTR (s3_OFFSET)"(sp)                      \n"
        "sw s4, "XTSTR (s4_OFFSET)"(sp)                      \n"
        "sw s5, "XTSTR (s5_OFFSET)"(sp)                      \n"
        "sw s6, "XTSTR (s6_OFFSET)"(sp)                      \n"
        "sw s7, "XTSTR (s7_OFFSET)"(sp)                      \n"
        "sw s8, "XTSTR (s8_OFFSET)"(sp)                      \n"
        "sw s9, "XTSTR (s9_OFFSET)"(sp)                      \n"
        "sw s10, "XTSTR (s10_OFFSET)"(sp)                    \n"
        "sw s11, "XTSTR (s11_OFFSET)"(sp)                    \n"

        /* Grab mepc to save it to the stack */
        "csrr s2, mepc                                      \n"

        /* Save return PC in stack frame */
        "sw s2, "XTSTR (pc_OFFSET)"(sp)                      \n"

        /* Save stack pointer of current thread */
        "sw sp, "XTSTR (SP_OFFSET_IN_THREAD)"(s1)            \n"

        /* Context saving done, from here on the new thread is scheduled */
        "null_thread:                                       \n"

        /*  Get the new active thread (guaranteed to be non NULL) */
        "lw s1, sched_active_thread                         \n"

        /*  Load the thread SP of scheduled thread */
        "lw sp, "XTSTR (SP_OFFSET_IN_THREAD)"(s1)            \n"

        /*  Set return PC to mepc */
        "lw a1, "XTSTR (pc_OFFSET)"(sp)                      \n"
        "csrw mepc, a1                                      \n"

        /* restore s2-s11 */
        "lw s2, "XTSTR (s2_OFFSET)"(sp)                      \n"
        "lw s3, "XTSTR (s3_OFFSET)"(sp)                      \n"
        "lw s4, "XTSTR (s4_OFFSET)"(sp)                      \n"
        "lw s5, "XTSTR (s5_OFFSET)"(sp)                      \n"
        "lw s6, "XTSTR (s6_OFFSET)"(sp)                      \n"
        "lw s7, "XTSTR (s7_OFFSET)"(sp)                      \n"
        "lw s8, "XTSTR (s8_OFFSET)"(sp)                      \n"
        "lw s9, "XTSTR (s9_OFFSET)"(sp)                      \n"
        "lw s10, "XTSTR (s10_OFFSET)"(sp)                    \n"
        "lw s11, "XTSTR (s11_OFFSET)"(sp)                    \n"

        "no_switch:                                         \n"

        /* restore the caller-saved registers */
        "lw ra, "XTSTR (ra_OFFSET)"(sp)                      \n"
        "lw t0, "XTSTR (t0_OFFSET)"(sp)                      \n"
        "lw t1, "XTSTR (t1_OFFSET)"(sp)                      \n"
        "lw t2, "XTSTR (t2_OFFSET)"(sp)                      \n"
        "lw t3, "XTSTR (t3_OFFSET)"(sp)                      \n"
        "lw t4, "XTSTR (t4_OFFSET)"(sp)                      \n"
        "lw t5, "XTSTR (t5_OFFSET)"(sp)                      \n"
        "lw t6, "XTSTR (t6_OFFSET)"(sp)                      \n"
        "lw a0, "XTSTR (a0_OFFSET)"(sp)                      \n"
        "lw a1, "XTSTR (a1_OFFSET)"(sp)                      \n"
        "lw a2, "XTSTR (a2_OFFSET)"(sp)                      \n"
        "lw a3, "XTSTR (a3_OFFSET)"(sp)                      \n"
        "lw a4, "XTSTR (a4_OFFSET)"(sp)                      \n"
        "lw a5, "XTSTR (a5_OFFSET)"(sp)                      \n"
        "lw a6, "XTSTR (a6_OFFSET)"(sp)                      \n"
        "lw a7, "XTSTR (a7_OFFSET)"(sp)                      \n"
        "lw s0, "XTSTR (s0_OFFSET)"(sp)                      \n"
        "lw s1, "XTSTR (s1_OFFSET)"(sp)                      \n"

        "addi sp, sp, "XTSTR (CONTEXT_FRAME_SIZE)"           \n"
        :
        :
        :
        );
}
