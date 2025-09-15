MODULE = cpu

ifeq ($(RP2350_RISCV), $(RP2350_ARM))
$(error Neither (or both) rp2350_arm and rp2350_riscv are defined.)
endif

DIRS += $(RIOTCPU)/rp2350/periph

ifdef RP2350_ARM
DIRS += $(RIOTCPU)/cortexm_common
else
DIRS += $(RIOTCPU)/riscv_common
endif

$(info DIRS are $(DIRS))

include $(RIOTBASE)/Makefile.base
