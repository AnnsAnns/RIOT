# Dabao (Baochip-1x)

## Overview

The [Dabao](https://www.crowdsupply.com/baochip/dabao) is an evaluation board
for the [Baochip-1x](https://baochip.com/) SoC.

## Hardware

- Baochip-1x, TSMC 22nm
- 350 MHz VexRiscv RV32-IMAC core with Sv39 MMU
- 4 MiB RRAM (non-volatile, execute-in-place)
- 2 MiB SRAM
- 128 KiB IFRAM (DMA-accessible buffer memory)
- quad-core 700 MHz PicoRV32 "BIO" I/O coprocessor
- USB 2.0 high-speed device
- no on-board LED

## Flashing

The board ships with `bootwait` enabled and exposes a `boot1` REPL over the
USB CDC interface (1 Mbaud). Images are signed and transferred as UF2 blocks.
