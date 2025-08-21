MODULE = cpu

abnoapsd

# Check if both modules are defined
ifneq (,$(filter rp2350_arm rp2350_riscv,$(USEMODULE)))
		ifneq (,$(filter rp2350_arm,$(USEMODULE)))
				ifneq (,$(filter rp2350_riscv,$(USEMODULE)))
						$(error Both rp2350_arm and rp2350_riscv are defined. Only one can be enabled at a time.)
				endif
		endif
endif

# Check if neither module is defined
ifeq (,$(filter rp2350_arm rp2350_riscv,$(USEMODULE)))
		$(error Neither rp2350_arm nor rp2350_riscv is defined. One must be enabled via USEMODULE.)
endif

ifeq (,$(filter rp2350_arm,$(USEMODULE)))
		DIRS += $(RIOTCPU)/cortexm_common
else
		DIRS += $(RIOTCPU)/riscv_common
endif

DIRS += periph

include $(RIOTBASE)/Makefile.base
