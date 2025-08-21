MODULE = cpu

ifeq (,$(filter rp2350_arm,$(USEMODULE)))
DIRS += $(RIOTCPU)/cortexm_common
else
DIRS += $(RIOTCPU)/riscv_common
endif

DIRS += periph

include $(RIOTBASE)/Makefile.base
