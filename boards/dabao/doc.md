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
USB CDC interface (1 Mbaud). `make flash` signs the RIOT binary with the
Baochip signature block format and uploads it as UF2 blocks.

Host prerequisites: the flasher declares its dependencies as PEP 723 inline
script metadata and runs through `pipx` (the shebang is
`#!/usr/bin/env -S pipx run`). Install `pipx` if needed:

```
sudo apt install pipx          # or: python3 -m pip install --user pipx
```

`make flash` then runs the script directly; `pipx` installs the declared
`pyserial` and `PyNaCl` dependencies into an isolated environment on first use.
For local development without `pipx`, use pip >= 26:

```
pip install --requirements-from-script dist/tools/baochip/bao1x_flash.py
```

Then:

```
make BOARD=dabao flash
```

By default the board is left with `bootwait` enabled: the freshly uploaded
image runs, and pressing **RESET** brings back the `boot1` REPL for the next
`make flash`.

- To make the application run on every reset/power cycle, use
  `make BOARD=dabao flash BAO1X_PERSISTENT=1`. To reflash such a board, hold
  **PROG**, tap **RESET**, release **PROG**.
- To use a production signing key instead of the public development key:

  ```
  make BOARD=dabao flash BAO1X_SIGNING_KEY=/path/to/seed.bin
  ```

  The key file must contain a 32-byte Ed25519 seed (raw, PEM/PKCS#8, or a
  hex string via `--key-hex` when calling the flasher directly).

- If the board is not reachable, `make BOARD=dabao reset` sends the boot1
  `reset` command. It cannot reset a board that is running an application;
  use the physical RESET button for that.

The flasher prefers the stable `/dev/serial/by-id/usb-Baochip_*` link on
Linux. If the board shows up under a different name, set `PORT` explicitly or
use `MOST_RECENT_PORT=1` to let RIOT pick the correct tty:

```
make BOARD=dabao MOST_RECENT_PORT=1 flash
```

### Console

The RIOT application console is a USB CDC ACM device on the USB-C port. It
enumerates as `1d50:6197` (`dabao`) and shows up as `/dev/ttyACM*` on Linux or
`/dev/tty.usbmodem*` on macOS. `make term` selects the port automatically
after flashing:

```
make BOARD=dabao term
```

The application re-enumerates after boot1 hands over the USB controller, so the
`ttyACM` number can change once during boot. If you have several boards
connected, use `MOST_RECENT_PORT=1`.

The boot1 REPL keeps the Baochip PID `1d50:6196` and runs at 1 Mbaud; the
application uses RIOT's normal CDC ACM settings, so no baud rate has to be set.

The UART2 pins are still available as a secondary serial port:

| Signal | Pin |
|---|---|
| UART2 TX | PB14 |
| UART2 RX | PB13 |
| GND | GND |

### Manual flashing

The flasher can also produce a signed UF2 without touching the board:

```
python3 ../../dist/tools/baochip/bao1x_flash.py --uf2-only \
    bin/dabao/<app>.bin
```
