---
title: Porting a C Module to Rust
description: Rewrite a C module in Rust and test it from C and Rust on native
---

This guide shows how to rewrite a C module in Rust.
It uses `sys/uri_parser` as the example.
The Rust code keeps the C API, so C applications can still use the module.
A Rust test and a C test run the same module on the native board.

## How the build fits together

A Rust module is a Cargo crate.
The crate exposes normal C functions with `#[no_mangle]` and `extern "C"`.
The build system adds the crate to `rust_riotmodules`.
That crate collects all Rust modules that are built into the application.
The top-level crate builds a static library.
A C application links the library, and a Rust application uses the crate
directly.

The C API is the set of function names and data layouts in the header.
Keep the header and the C layout unchanged.
Use `#[repr(C)]` for each C struct in Rust.

## Step 1: Add the crate manifest

Create `Cargo.toml` in the module directory.
The crate is a normal library, not a static library.
The top-level crate provides the static library.

```toml
[package]
name = "riot-module-uri-parser"
version = "0.1.0"
edition = "2021"
license = "LGPL-2.1-only"
publish = false
```

## Step 2: Write the Rust code

Create `src/lib.rs`.
Start the file with `#![no_std]`.
Use `core::ffi::c_char` and `core::ffi::c_int` for C types.
Keep the safe Rust logic in normal functions.
Keep the `unsafe` code at the C boundary.

Mark each exported function with `#[no_mangle]` and `pub unsafe extern "C"`.
The function name must match the C header.

```rust
#[no_mangle]
pub unsafe extern "C" fn uri_parser_process(
    result: *mut UriParserResultC,
    uri: *const c_char,
    uri_len: usize,
) -> c_int {
    // check the pointers, then call the safe parser
}
```

The C structures in `src/lib.rs` must use the same field order and types as
the header.
For example, `uri_parser_result_t` has eight pointers, one `uint16_t` port,
and eight `uint16_t` lengths.

## Step 3: Register the crate

Add the crate to `sys/rust_riotmodules`.
The dependency is optional.
Add the crate to the `_all` feature too.

```toml
# sys/rust_riotmodules/Cargo.toml
riot-module-uri-parser = { path = "../uri_parser", optional = true }

[features]
_all = [ "riot-module-uri-parser" ]
```

Re-export the crate from `sys/rust_riotmodules/src/lib.rs`.
The feature name uses hyphens.
The Rust module name uses underscores.

```rust
#[cfg(feature = "riot-module-uri-parser")]
pub use riot_module_uri_parser as uri_parser;
```

Add the feature mapping to `makefiles/cargo-targets.inc.mk`.
The build system enables the feature only when the RIOT module is used.

```make
ifneq (,$(filter uri_parser,$(USEMODULE)))
  CARGO_OPTIONS += --features rust_riotmodules/riot-module-uri-parser
endif
```

## Step 4: Declare the module

A module with no C source has no `Makefile`.
A pseudomodule is a module that the build system does not compile directly.
Declare it as a pseudomodule in `sys/Makefile.include`.

```make
PSEUDOMODULES += uri_parser
```

Add the Rust module dependency to `sys/uri_parser/Makefile.dep`.

```make
USEMODULE += rust_riotmodules
```

## Step 5: Add a Rust test

Create a Rust application under `tests/sys/rust_uri_parser`.
The `Makefile` sets `APPLICATION_RUST_MODULE`.
It also adds the module under test.

```make
include ../Makefile.sys_common

USEMODULE += uri_parser

APPLICATION_RUST_MODULE = rust_uri_parser
FEATURES_REQUIRED += rust_target

include $(RIOTBASE)/Makefile.include
```

The `Cargo.toml` depends on `rust_riotmodules` with a relative path.
The code uses `riot_main!` and prints `SUCCESS` when all checks pass.
Call the safe Rust API directly.
Do not call the C functions from the Rust test.

## Step 6: Add a C test

Create a C application under `tests/sys/uri_parser`.
The `Makefile` adds the same module.

```make
include ../Makefile.sys_common

USEMODULE += uri_parser

include $(RIOTBASE)/Makefile.include
```

The C test includes `uri_parser.h` and calls the normal C API.
The linker finds the functions in the Rust static library.
This test proves that the C ABI still works.

## Step 7: Build and run on native

Use the RIOT Docker image.
This image has Rust, Cargo, and C2Rust.

```shell
make -C tests/sys/uri_parser BOARD=native BUILD_IN_DOCKER=1
./tests/sys/uri_parser/bin/native64/tests_uri_parser.elf
```

```shell
make -C tests/sys/rust_uri_parser BOARD=native BUILD_IN_DOCKER=1
./tests/sys/rust_uri_parser/bin/native64/tests_rust_uri_parser.elf
```

A 64-bit host builds the `native64` board.
The `native` board name selects that board automatically.
A 32-bit host uses `native32`.

The test directories contain a `tests/01-run.py` file.
The build system then sets `NATIVE_AUTO_EXIT=1`.
The native application exits after `main` returns.
Without a test script, the native application keeps running.

## Step 8: Run the old unit tests

The old unit tests check the old C behavior.
Run them after the rewrite.

```shell
make -C tests/unittests BOARD=native BUILD_IN_DOCKER=1
./tests/unittests/bin/native64/tests_unittests.elf
```

The `uri_parser` tests must pass.
Other test failures on native can come from unrelated modules.

## Limits

The module now needs the Rust toolchain for every build that uses it.
A C structure that does not match the Rust structure causes memory corruption.
Keep the C boundary small and test it on native first.
A hardware test still needs the target board.
