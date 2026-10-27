#!/usr/bin/env -S pipx run

# /// script
# requires-python = ">=3.9"
# dependencies = [
#   "pyserial>=3.5",
#   "PyNaCl>=1.5",
# ]
# ///

"""
Program a Baochip-1x (Dabao) through the boot1 USB serial REPL.

boot1 only boots signed images. This tool signs the RIOT ``.bin`` with the
Baochip signature block format, packs it into UF2 blocks and uploads them with
the ``uf2`` command of the boot1 REPL.

The default key is the public Baochip *development* key. It is fine for
development and lab use, but it is not a secret: do not use it for production
devices. Pass ``--key`` (or ``BAO1X_SIGNING_KEY`` in the build system) to use
your own 32-byte Ed25519 seed.

Dependencies are declared as PEP 723 inline script metadata, so the script can
be run directly through ``pipx`` (the shebang above) or with
``pip install --requirements-from-script bao1x_flash.py`` (pip >= 26).

SPDX-FileCopyrightText: 2026 RIOT contributors
SPDX-License-Identifier: LGPL-2.1-only
"""

import argparse
import base64
import os
import struct
import sys
import time

try:
    import serial
except ImportError:  # pragma: no cover - depends on the host
    sys.exit(
        "error: pyserial is required (pip install pyserial / "
        "apt install python3-serial)"
    )

_NACL = None


def _nacl():
    """Import PyNaCl lazily, so --check/--reset work without it."""
    global _NACL
    if _NACL is None:
        try:
            from nacl.bindings import (
                crypto_sign_ed25519ph_final_create,
                crypto_sign_ed25519ph_state,
                crypto_sign_ed25519ph_update,
                crypto_sign_seed_keypair,
            )
        except ImportError:  # pragma: no cover - depends on the host
            sys.exit(
                "error: PyNaCl is required for Ed25519ph signing "
                "(pip install pynacl / apt install python3-nacl)"
            )
        _NACL = (
            crypto_sign_seed_keypair,
            crypto_sign_ed25519ph_state,
            crypto_sign_ed25519ph_update,
            crypto_sign_ed25519ph_final_create,
        )
    return _NACL


def public_key(seed):
    """Derive the Ed25519 public key from a 32-byte seed."""
    seed_keypair = _nacl()[0]
    return seed_keypair(seed)[0]


def sign_ph(seed, message):
    """Return ``(signature, public_key)`` for an Ed25519ph signature."""
    seed_keypair, ph_state, ph_update, ph_final_create = _nacl()
    pub, secret = seed_keypair(seed)
    state = ph_state()
    for offset in range(0, len(message), 64 * 1024):
        ph_update(state, message[offset:offset + 64 * 1024])
    return ph_final_create(state, secret), pub


# boot1 REPL defaults
DEFAULT_PORT_LINUX = "/dev/ttyACM0"
DEFAULT_PORT_DARWIN = "/dev/tty.usbmodem"
DEFAULT_BAUD = 1000000
DEFAULT_BASE = 0x60060000
DEFAULT_UF2_FAMILY = 0xA7D76373

# signature block layout (see bao1x-api/src/signatures.rs)
SIG_BLOCK_LEN = 0x300
SIG_VERSION = 0x100
# The on-wire magic is the two ASCII words stored as little-endian u32s, i.e.
# the byte sequence is "ymuy3oaB" even though the logical string is "yumyBao3".
SIG_MAGIC0 = int.from_bytes(b"yumy", "big")
SIG_MAGIC1 = int.from_bytes(b"Bao3", "big")
FUNC_BAREMETAL = 6
ANTI_ROLLBACK = 1
PUBKEY_TAG = b"dev "
UNSIGNED_LEN = 132
SEALED_FIELDS_SIZE = 220
PADDING_SIZE = SIG_BLOCK_LEN - UNSIGNED_LEN - SEALED_FIELDS_SIZE

# UF2 constants
UF2_MAGIC0 = 0x0A324655
UF2_MAGIC1 = 0x9E5D5157
UF2_FLAG_FAMILY = 0x00002000
UF2_MAGIC_END = 0x0AB16F30
UF2_PAYLOAD = 256

# The development key published with the dabao SDK. Not a secret.
DEV_SEED = bytes.fromhex(
    "a8a7765c8da2d793862b08b000debca7b20e89ed52c785a2b3eba1db9cb62a27"
)


def make_jal(offset):
    """Encode a RISC-V ``jal x0, offset`` instruction."""
    if offset % 2 or not -(1 << 20) <= offset < (1 << 20):
        raise ValueError("JAL offset out of range")
    imm = offset & 0x1FFFFF
    instr = ((imm >> 20) & 1) << 31
    instr |= ((imm >> 1) & 0x3FF) << 21
    instr |= ((imm >> 11) & 1) << 20
    instr |= ((imm >> 12) & 0xFF) << 12
    instr |= 0x6F
    return struct.pack("<I", instr)


def build_signature_block(code, seed, key_tag=PUBKEY_TAG):
    """Build the 0x300-byte signature block."""
    if len(key_tag) != 4:
        raise ValueError("key tag must be exactly 4 bytes")
    pub = public_key(seed)

    sealed = b""
    sealed += struct.pack("<I", SIG_VERSION)
    sealed += struct.pack("<II", SIG_MAGIC0, SIG_MAGIC1)
    sealed += struct.pack("<I", SEALED_FIELDS_SIZE + PADDING_SIZE + len(code))
    sealed += struct.pack("<I", FUNC_BAREMETAL)
    sealed += struct.pack("<I", ANTI_ROLLBACK)
    sealed += bytes(16)  # min_semver
    sealed += bytes(16)  # semver
    sealed += bytes(36)  # pubkey slot 0
    sealed += bytes(36)  # pubkey slot 1
    sealed += bytes(36)  # pubkey slot 2
    sealed += pub + key_tag  # pubkey slot 3 (developer)
    sealed += bytes(20)  # toolchain
    assert len(sealed) == SEALED_FIELDS_SIZE

    padding = bytes(PADDING_SIZE)
    signature = sign_ph(seed, sealed + padding + code)[0]
    assert len(signature) == 64

    block = make_jal(SIG_BLOCK_LEN)
    block += signature
    block += struct.pack("<I", 0)  # aad_len (0 = Ed25519ph)
    block += bytes(60)  # aad
    block += sealed
    block += padding
    assert len(block) == SIG_BLOCK_LEN
    return block


def pack_uf2(image, base, family):
    """Pack an image into UF2 blocks."""
    blocks = []
    count = (len(image) + UF2_PAYLOAD - 1) // UF2_PAYLOAD
    for index in range(count):
        offset = index * UF2_PAYLOAD
        chunk = image[offset:offset + UF2_PAYLOAD]
        if len(chunk) < UF2_PAYLOAD:
            chunk = chunk + bytes(UF2_PAYLOAD - len(chunk))
        header = struct.pack(
            "<IIIIIIII",
            UF2_MAGIC0,
            UF2_MAGIC1,
            UF2_FLAG_FAMILY,
            base + offset,
            UF2_PAYLOAD,
            index,
            count,
            family,
        )
        blocks.append(
            header + chunk + bytes(512 - 32 - UF2_PAYLOAD - 4)
            + struct.pack("<I", UF2_MAGIC_END)
        )
    return b"".join(blocks)


def load_seed(args):
    if args.key_hex:
        seed = bytes.fromhex(args.key_hex)
    elif args.key:
        with open(args.key, "rb") as f:
            data = f.read()
        if data.startswith(b"-----BEGIN"):
            body = b"".join(
                line for line in data.splitlines()
                if line and not line.startswith(b"-----")
            )
            data = base64.b64decode(body)
        # PKCS#8: the 32-byte seed is the final privateKey field.
        seed = data[-32:] if len(data) > 32 else data
    else:
        seed = DEV_SEED
        print(
            "note: using the public Baochip development signing key; "
            "pass --key for production use",
            file=sys.stderr,
        )
    if len(seed) != 32:
        sys.exit("error: Ed25519 seed must be exactly 32 bytes")
    return seed


def default_port():
    if sys.platform == "darwin":
        import glob
        ports = sorted(glob.glob(DEFAULT_PORT_DARWIN + "*"))
        return ports[0] if ports else DEFAULT_PORT_DARWIN + "0"
    return os.environ.get("PORT", DEFAULT_PORT_LINUX)


class Boot1:
    def __init__(self, port, baud, timeout=2.0):
        self.port = port
        self.ser = serial.Serial(port, baud, timeout=0.2)
        self.timeout = timeout

    def close(self):
        try:
            self.ser.close()
        except Exception:
            pass

    def _read_until(self, needles, timeout):
        data = b""
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            try:
                waiting = self.ser.in_waiting
            except Exception:
                break
            try:
                chunk = self.ser.read(waiting or 1)
            except Exception:
                break
            if chunk:
                data += chunk
                if any(n in data for n in needles):
                    break
        return data

    def command(self, command, timeout=None, needles=(b"\n",)):
        timeout = self.timeout if timeout is None else timeout
        self.ser.write(command.encode("ascii") + b"\r\n")
        self.ser.flush()
        return self._read_until(needles, timeout)

    def detect(self, retries=3):
        for _ in range(retries):
            try:
                self.ser.reset_input_buffer()
                data = self.command("", timeout=0.8)
            except Exception:
                time.sleep(0.3)
                continue
            if b"Commands include" in data or b"bootwait" in data:
                return data.decode("utf-8", "replace")
            time.sleep(0.3)
        return None

    def upload(self, uf2_data, verbose=False):
        blocks = len(uf2_data) // 512
        # The REPL echoes commands; turning echo off makes large uploads faster.
        try:
            self.command("localecho off", timeout=1.0)
        except Exception:
            pass
        print(f"Uploading {blocks} UF2 blocks to {self.port} ...")
        for index in range(blocks):
            block = uf2_data[index * 512:(index + 1) * 512]
            payload = base64.b64encode(block).decode("ascii")
            try:
                self.ser.write(("uf2 " + payload + "\r\n").encode("ascii"))
                self.ser.flush()
                response = self._read_until(
                    (b"Wrote", b"Invalid", b"Error", b"error"), 3.0
                )
            except Exception as exc:
                raise RuntimeError(
                    f"connection lost while writing block {index + 1}/{blocks}: "
                    f"{exc}"
                ) from exc
            text = response.decode("utf-8", "replace")
            if "Wrote" not in text:
                raise RuntimeError(
                    f"boot1 rejected block {index + 1}/{blocks}: "
                    f"{text.strip() or 'no response'}"
                )
            if verbose:
                address = struct.unpack_from("<I", block, 12)[0]
                print(f"  [{index + 1}/{blocks}] 0x{address:08x} - {text.strip()}")
            elif blocks >= 16 and (index + 1) % 16 == 0:
                print(f"  {index + 1}/{blocks} blocks", end="\r", flush=True)
        if blocks >= 16:
            print(" " * 40, end="\r")
        print(f"Wrote {blocks} blocks ({blocks * UF2_PAYLOAD} bytes)")

    def set_bootwait(self, enabled):
        state = "enable" if enabled else "disable"
        try:
            response = self.command(f"bootwait {state}", timeout=1.5)
        except Exception as exc:
            raise RuntimeError(f"failed to set bootwait {state}: {exc}") from exc
        if b"bootwait" in response and b"error" in response.lower():
            raise RuntimeError(
                f"boot1 refused 'bootwait {state}': "
                f"{response.decode('utf-8', 'replace').strip()}"
            )

    def start(self):
        try:
            self.ser.write(b"boot\r\n")
            self.ser.flush()
            time.sleep(0.2)
            self.ser.read(self.ser.in_waiting or 0)
        except Exception:
            # The device normally disconnects as soon as the image starts.
            pass


def reset_via_dtr(port):
    try:
        with serial.Serial(port, 115200, timeout=0.2) as ser:
            ser.dtr = True
            time.sleep(0.1)
            ser.dtr = False
    except Exception as exc:
        sys.exit(f"error: cannot toggle DTR on {port}: {exc}")
    time.sleep(2.0)


def main():
    parser = argparse.ArgumentParser(
        description="Flash a Baochip-1x (Dabao) via the boot1 USB REPL",
    )
    parser.add_argument("image", nargs="?",
                        help="RIOT .bin image linked at <base> + 0x300")
    parser.add_argument("--port", default=default_port(),
                        help="boot1 serial device (default: %(default)s)")
    parser.add_argument("--baud", type=int, default=DEFAULT_BAUD,
                        help="boot1 REPL baud rate (default: %(default)s)")
    parser.add_argument("--base", type=lambda x: int(x, 0), default=DEFAULT_BASE,
                        help="address of the signature block (default: 0x%x)"
                             % DEFAULT_BASE)
    parser.add_argument("--family", type=lambda x: int(x, 0),
                        default=DEFAULT_UF2_FAMILY,
                        help="UF2 family ID (default: 0x%x)" % DEFAULT_UF2_FAMILY)
    parser.add_argument("--key", help="32-byte raw or PEM Ed25519 signing key")
    parser.add_argument("--key-hex", help="Ed25519 signing seed as hex")
    parser.add_argument("--key-tag", default=PUBKEY_TAG.decode(),
                        help="4-byte key tag stored in pubkey slot 3 "
                             "(default: %(default)r)")
    parser.add_argument("--uf2-out", help="write the signed UF2 to this path")
    parser.add_argument("--no-uf2-out", action="store_true",
                        help="do not write a UF2 file")
    parser.add_argument("--uf2-only", action="store_true",
                        help="create the UF2 file but do not touch the board")
    parser.add_argument("--persistent", action="store_true",
                        help="disable bootwait, so the app runs after RESET")
    parser.add_argument("--no-boot", action="store_true",
                        help="leave the board in the boot1 REPL")
    parser.add_argument("--reset", action="store_true",
                        help="reset the board and exit")
    parser.add_argument("--check", action="store_true",
                        help="only check that the boot1 REPL responds")
    parser.add_argument("--reset-port",
                        help="optional second serial port whose DTR resets the "
                             "board before flashing (ICSP adapter)")
    parser.add_argument("--verbose", action="store_true")
    args = parser.parse_args()

    if args.key and args.key_hex:
        parser.error("--key and --key-hex are mutually exclusive")
    key_tag = args.key_tag.encode()
    if len(key_tag) != 4:
        parser.error("--key-tag must be exactly 4 bytes")

    if args.reset_port:
        reset_via_dtr(args.reset_port)

    if args.reset or args.check:
        try:
            boot1 = Boot1(args.port, args.baud)
        except Exception as exc:
            sys.exit(f"error: cannot open {args.port}: {exc}")
        try:
            banner = boot1.detect()
            if banner is None:
                sys.exit(
                    f"error: no boot1 REPL response on {args.port}\n"
                    "  Press RESET (bootwait enabled), or hold PROG, tap "
                    "RESET and release PROG."
                )
            if args.check:
                board = boot1.command("boardtype", timeout=1.0)
                print(board.decode("utf-8", "replace").strip())
                return
            boot1.command("reset", timeout=1.0)
        except Exception as exc:
            sys.exit(f"error: {exc}")
        finally:
            boot1.close()
        return

    if not args.image:
        parser.error("an image is required unless --check or --reset is used")

    with open(args.image, "rb") as f:
        code = f.read()
    if not code:
        sys.exit(f"error: {args.image} is empty")

    seed = load_seed(args)
    signature_block = build_signature_block(code, seed, key_tag)
    image = signature_block + code
    uf2 = pack_uf2(image, args.base, args.family)

    uf2_path = args.uf2_out
    if uf2_path is None and not args.no_uf2_out:
        uf2_path = os.path.splitext(args.image)[0] + ".uf2"
    if uf2_path:
        with open(uf2_path, "wb") as f:
            f.write(uf2)
        print(f"Signed image: {len(image)} bytes -> {uf2_path}")

    if args.uf2_only:
        return

    try:
        boot1 = Boot1(args.port, args.baud)
    except Exception as exc:
        sys.exit(
            f"error: cannot open {args.port}: {exc}\n"
            "  Is the Dabao connected, and is your user in the 'dialout' "
            "group?"
        )
    try:
        if boot1.detect() is None:
            raise RuntimeError(
                f"no boot1 REPL response on {args.port}\n"
                "  Press RESET (bootwait enabled), or hold PROG, tap RESET "
                "and release PROG.\n"
                "  If a previous app disabled bootwait, use the PROG+RESET "
                "sequence."
            )
        boot1.upload(uf2, verbose=args.verbose)
        boot1.set_bootwait(not args.persistent)
        if not args.no_boot:
            boot1.start()
        print("Done. Board is running the new firmware." if not args.no_boot
              else "Done. Image written; board left in boot1.")
    except KeyboardInterrupt:
        sys.exit("\ninterrupted")
    except Exception as exc:
        sys.exit(f"error: {exc}")
    finally:
        boot1.close()


if __name__ == "__main__":
    main()
