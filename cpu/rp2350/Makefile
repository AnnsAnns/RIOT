MODULE = cpu

ifeq ($(RP2350_ARM), 1)
DIRS += $(RIOTCPU)/cortexm_common
else
DIRS += $(RIOTCPU)/riscv_common
endif

DIRS += periph

include $(RIOTBASE)/Makefile.base
