# name of your application
APPLICATION = ssd1306_i2c

# If no BOARD is found in the environment, use this default:
BOARD ?= rpi-pico-2-arm

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# The display is driven via the U8g2 graphics package
USEPKG += u8g2

# u8x8_riotos uses the hardware I2C bus of the board
FEATURES_REQUIRED += periph_i2c

# used to pace the screen changes
USEMODULE += ztimer
USEMODULE += ztimer_usec

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
