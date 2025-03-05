#!/bin/sh

# $1 contains generated elf file - $FLASHFILE from makefile

ELFFILE="$1"
UF2FILE="${UF2FILE:-${ELFFILE%%.elf}.uf2}"
PICOTOOL_MOUNT_PATH="${PICOTOOL_MOUNT_PATH:-/media/$USER/RPI-RP2}"

"$(dirname "$0")"/picotool uf2 convert "$ELFFILE" "$UF2FILE"

cp "$UF2FILE" "$PICOTOOL_MOUNT_PATH"
