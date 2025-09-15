MODULE = cpu

ifeq ($(RP2350_RISCV), $(RP2350_ARM))
$(error Neither (or both) rp2350_arm and rp2350_riscv are defined.)
endif

ifdef RP2350_ARM
DIRS += $(RIOTCPU)/cortexm_common
else
DIRS += $(RIOTCPU)/riscv_common
endif

DIRS += $(RIOTCPU)/rp2350/periph

$(info DIRS are $(DIRS))

include $(RIOTBASE)/Makefile.base
