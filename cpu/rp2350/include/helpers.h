#pragma once

#include "RP2350.h"

#define ATOMIC_XOR_WRITE 0x1000
#define ATOMIC_BITMASK_SET_WRITE 0x2000
#define ATOMIC_BITMASK_CLEAR_WRITE 0x3000

static inline void write_register(volatile uint32_t *reg, uint32_t val)
{
    *(volatile uint32_t *)reg = val;
}

/**
 * @brief Perform an atomic XOR write to a register
 *
 * @param[in] reg   Pointer to the target register
 * @param[in] val   Value to be XORed with the register
 */
static inline void atomic_xor_write(volatile uint32_t *reg, uint32_t val)
{
    *(volatile uint32_t *)((uintptr_t)reg | ATOMIC_XOR_WRITE) = val;
}

/**
 * @brief Set bits in a register atomically
 *
 * @param[in] reg   Pointer to the target register
 * @param[in] val   Bit mask of bits to set
 */
static inline void atomic_bitmask_set(volatile uint32_t *reg, uint32_t val)
{
    *(volatile uint32_t *)((uintptr_t)reg | ATOMIC_BITMASK_SET_WRITE) = val;
}

/**
 * @brief Clear bits in a register atomically
 *
 * @param[in] reg   Pointer to the target register
 * @param[in] val   Bit mask of bits to clear
 */
static inline void atomic_bitmask_clear(volatile uint32_t *reg, uint32_t val)
{
    *(volatile uint32_t *)((uintptr_t)reg | ATOMIC_BITMASK_CLEAR_WRITE) = val;
}
