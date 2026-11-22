---
title: Build in Nix
description: Building RIOT in a Nix-managed development environment
---

RIOT provides a [Nix flake](https://nixos.wiki/wiki/Flakes) that contains the
cross toolchains, static analysis tools, and host utilities needed to build
RIOT. It is an alternative to the [Docker build
environment](/build-system/build-in-docker/).

The flake lives in the repository root (`flake.nix`), the helper derivations
are under `dist/nix/`. Nix support is still highly experimental.

## Requirements

- Nix with flakes enabled (Nix 2.4 or newer)
- A Linux host: `x86_64-linux` has the full toolchain set, `aarch64-linux`
  supports the host, Arm, AVR, and ESP32 toolchains

### Enabling Flakes

Nix still ships flakes and the `nix` command line as experimental features.
If `nix develop` fails with:

```text
experimental Nix feature 'flakes' is disabled
```

enable both features in `~/.config/nix/nix.conf` (or `/etc/nix/nix.conf`):

```ini
experimental-features = nix-command flakes
```

On NixOS, set the same option declaratively:

```nix
nix.settings.experimental-features = [ "nix-command" "flakes" ];
```

The Docker image remains the recommended option for exact reproduction of the
official CI environment.

## Automatic Environment Selection

Instead of entering a shell manually, set `BUILD_IN_NIX=1`:

```shell
BUILD_IN_NIX=1 make -C examples/basic/hello-world BOARD=rpi-pico
BUILD_IN_NIX=1 make -C examples/basic/hello-world BOARD=native64 flash
```

The CPU declares the environment in its `Makefile.include`, e.g. `NIX_ENV=rp-pico` for the RP2040 and RP2350.

The environment can be overridden explicitly:

```shell
BUILD_IN_NIX=1 NIX_ENV=nativeFull make -C examples/basic/hello-world BOARD=native64
```

Show the selected environment without entering it:

```shell
make -C examples/basic/hello-world BOARD=rpi-pico BUILD_IN_NIX=1 info-nix-env
```

Some environments are selected from application or board properties:

- Native applications using LVGL or SDL-backed U8g2/Ucglib displays use
  `nativeFull`.
- Boards emulated with QEMU (`EMULATE=1`) use the `default` shell.

`BUILD_IN_DOCKER` is not needed and should not be set inside the Nix shell.

## Entering the Development Shell Manually

From the RIOT repository root:

```shell
nix develop
```

The default shell contains all supported cross toolchains and the static test
tools. Smaller shells are available per target family:

```shell
nix develop .#native       # native32/native6
nix develop .#rp-pico      # RP2040 and RP2350 (Arm and RISC-V)
nix develop .#stm32        # STM32
nix develop .#esp          # ESP8266 and ESP32
nix develop .#avr          # AVR
nix develop .#static-test  # static tests only
[...]
nix flake show             # list all environments
```

If you have a tool such as `direnv` installed, it should also automatically
recognize and apply the nix development environment.

### Building

Once inside the shell, use `make` as usual:

```shell
make -C examples/basic/hello-world BOARD=native64
make -C examples/basic/hello-world BOARD=samr21-xpro
make -C examples/basic/hello-world BOARD=arduino-uno
```

For non-interactive use, pass the command to `nix develop`:

```shell
nix develop --command make -C examples/basic/hello-world BOARD=native64
```

## Static Tests

The `static-test` shell contains the tools used by `make static-test`:

```shell
nix develop .#static-test --command ./dist/tools/ci/static_tests.sh
```

The script expects a Git base branch (or `CI_BASE_BRANCH`) as in CI.

## Updating the Environment

The flake pins nixpkgs and the ESP toolchains in `flake.lock`. Update them with:

```shell
nix flake update
```

Format the Nix files with:

```shell
nix fmt
```

Validate the flake and build the `native64` smoke test with:

```shell
nix flake check
```

The Nix build-test CI workflow also builds representative native, Arm, AVR,
cross-LLVM, LVGL, and U8g2 targets through `BUILD_IN_NIX=1`, so the shells are
exercised instead of only evaluated.

## Limitations

- `native32` is only available on `x86_64-linux`. SDL-backed `native32`
  applications additionally need a 32-bit SDL2, which is not part of the
  current shell.
- The RISC-V, MSP430, and ESP8266 toolchains are prebuilt `x86_64-linux`
  binaries; they are not available on `aarch64-linux`. The ESP32 toolchains
  are available on aarch64, so `.#esp` exists there but contains ESP32 only.
- The ESP8266 toolchain's GDB is not included. It depends on Python 2.7 and an
  old ncurses ABI that are not available in current nixpkgs. Use an external
  GDB for ESP8266 debugging.
- LLVM support follows RIOT's CPU support, but some boards still have
  compiler-specific failures. In particular, `rpi-pico-2-*` (RP2350)
  currently build with GCC but not with `TOOLCHAIN=llvm`.
- `c2rust` is not packaged in nixpkgs and has to be installed with `cargo`
  (see above).
- The picolibc variants built into riotdocker are not part of the Nix shell.
  Use the Docker image for builds that require `PICOLIBC=1`.
- Flashing and debugging real hardware still requires the usual udev rules
  and permissions for the debug adapter.
