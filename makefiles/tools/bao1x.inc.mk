# Configure the Baochip-1x boot1 programmer
#
# boot1 only boots signed images, so the flasher both signs and uploads.
# See dist/tools/baochip/bao1x_flash.py.

BAO1X_FLASHER ?= $(RIOTTOOLS)/baochip/bao1x_flash.py

FLASHER ?= $(BAO1X_FLASHER)
FLASHFILE ?= $(BINFILE)
PROG_BAUD ?= 1000000

FFLAGS ?= --port "$(PROG_DEV)" --baud $(PROG_BAUD)
FFLAGS += $(if $(BAO1X_SIGNING_KEY),--key "$(BAO1X_SIGNING_KEY)")
FFLAGS += $(if $(BAO1X_KEY_TAG),--key-tag "$(BAO1X_KEY_TAG)")
FFLAGS += $(if $(filter 1,$(BAO1X_PERSISTENT)),--persistent)
FFLAGS += $(if $(BAO1X_RESET_PORT),--reset-port "$(BAO1X_RESET_PORT)")
FFLAGS += "$(FLASHFILE)"

RESET ?= $(BAO1X_FLASHER)
RESET_FLAGS ?= --port "$(PROG_DEV)" --reset

FLASHDEPS += $(BAO1X_FLASHER)
