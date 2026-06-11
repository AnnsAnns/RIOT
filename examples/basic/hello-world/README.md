SSD1306 over I2C on the RP2350
==============================

This example drives an SSD1306 based 128x64 OLED display with the U8g2
graphics package over the hardware I2C bus of the RP2350 (Raspberry Pi
Pico 2). It cycles through three screens (two text screens and the
RIOT-OS logo), one per second.

Wiring
------

On the `rpi-pico-2-arm` / `rpi-pico-2-riscv` boards the first I2C bus is
I2C0 with:

| Signal | Pin   |
|--------|-------|
| SDA    | GPIO4 |
| SCL    | GPIO5 |

Connect the display's SDA/SCL to these pins, VCC to 3V3 and GND to GND.
The driver enables the internal pull-ups of the pads, so a bare display
breakout without its own pull-up resistors will work as well.

The display is expected at I2C address `0x3c` (the usual default for
SSD1306 breakouts). The address and bus can be overridden at compile
time, e.g.:

    make BOARD=rpi-pico-2-arm CFLAGS="-DSSD1306_I2C_ADDR=0x3d"

Usage
-----

Build and flash with:

    make BOARD=rpi-pico-2-arm flash
