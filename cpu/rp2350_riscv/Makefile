$(info Makefile picoarch is $(PICOARCH))

PICOARCH ?= RISCV

ifeq ($(PICOARCH),RISCV)

MODULE = cpu

DIRS += $(RIOTCPU)/riscv_common
DIRS += periph

include $(RIOTBASE)/Makefile.base

else ifeq ($(PICOARCH),ARM)

MODULE = cpu

DIRS += $(RIOTCPU)/cortexm_common
DIRS += periph

include $(RIOTBASE)/Makefile.base

endif
