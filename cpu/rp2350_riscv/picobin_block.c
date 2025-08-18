/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * Picobin block required for the binary
 * This defines the minimum viable image metadata to be recognized by the RP2350 bootloader
 * based on RP2350 Chapter 5.9.1
 */

#include <stdint.h>

/* Create a section for the picobin block that can be moved by the linker */
__attribute__((section(".picobin_block")))
const uint32_t picobin_block[] = {
    /* PICOBIN_BLOCK_MARKER_START */
    0xffffded3,

    /* Combine item 0 into a single 32-bit word
     * - byte 0: 0x42 (size_flag == 0, item_type == PICOBIN_BLOCK_ITEM_1BS_IMAGE_TYPE)
     * - byte 1: 0x1 (Block Size in words)
     * - bytes 2-3: image_type_flags (2 bytes) [See 5.9.3.1 / p419]
     *   - 15 -> 0 (1 for "Try before you buy" image [Wacky]
     *   - 12-14 -> 001 (RP2350 = 1)
     *   - 11 -> 0 (Reserved)
     *   - 8-10 -> 001 (EXE_CPU_ARM = 0), **WARNING** if I ever want to support RISC-V this needs to be 001 (EXE_CPU_RISCV)
     *   - 6-7 -> 00 (Reserved)
     *   - 4-5 -> 10 (2) EXE Security (As far as I understand we cant run in EXE_SECURITY_NS on the RP2350 [Appears to be correct]) thus EXE_SECURITY_S = 2
     *   - 0-3 // 0001 IMAGE_TYPE_EXE
     */
    (0x42) | (0x01 << 8) | (0b0001000000100001 << 16),

    /* Combine the last item into a single 32-bit word
     * - byte 0: 0xff (PICOBIN_BLOCK_ITEM_2BS_LAST)
     * - bytes 1-2: 0x0001 (Size of the item in words - predefined value)
     * - byte 3: 0x00 (Padding)
     */
    (0xff) | (0x0001 << 8) | (0x00 << 24),

    /* Next Block Pointer */
    0x00000000, /* Next block pointer (0 means no more blocks) */

    /* PICOBIN_BLOCK_MARKER_END */
    0xab123579  /* Marker for the end of the picobin block */
};
