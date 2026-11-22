# Nix build environment support.
#
# BUILD_IN_NIX=1 re-executes the complete make invocation inside the Nix
# development shell selected by NIX_ENV.  Unlike the Docker integration this
# also wraps flash, term, debug and test

NIX ?= nix
NIX_FLAKE ?= $(RIOTBASE)
NIX_DEVELOP_FLAGS ?=

# NIX_ENV is normally set by the CPU Makefile.include.  Native applications
# with SDL-backed displays need the larger native shell.
# Thus we sort for them here.
# @TODO: Find cleaner solution
ifeq (native,$(NIX_ENV))
  ifneq (,$(filter lvgl lv_drivers u8g2 ucglib,$(USEPKG)))
    NIX_ENV := nativeFull
  endif
  ifneq (,$(filter lv_drivers_sdl u8g2_sdl ucglib_sdl,$(USEMODULE)))
    NIX_ENV := nativeFull
  endif
endif

# QEMU is only part of the full default shell.
ifeq (1,$(EMULATE))
  ifeq (qemu,$(RIOT_EMULATOR))
    NIX_ENV := default
  endif
endif

# Fallback when nothing selected an environment.
NIX_ENV ?= default

.PHONY: ..in-nix-shell info-nix-env

# Command-line variable assignments are propagated through MAKEFLAGS.
NIX_INNER_GOALS := $(filter-out ..in-nix-shell info-nix-env,$(MAKECMDGOALS))
NIX_WRAPPED_GOALS := $(filter-out info-%,$(NIX_INNER_GOALS))
NIX_MAKE_JOBS := $(filter -j%,$(MAKEFLAGS))
NIX_INNER_MAKEFLAGS := $(filter-out -j% --jobserver-% BUILD_IN_NIX=% INSIDE_NIX=%,$(MAKEFLAGS))

ifeq (1,$(_NIX_WRAP_EARLY))
  NIX_WRAP := 1
else
  NIX_WRAP := 0
endif

ifeq (1,$(NIX_WRAP))
  .NOTPARALLEL:
  ifeq (,$(NIX_WRAPPED_GOALS))
    .DEFAULT_GOAL := ..in-nix-shell
  else
    $(foreach goal,$(NIX_WRAPPED_GOALS),$(eval $(goal): ..in-nix-shell ; @:))
    .DEFAULT_GOAL := $(firstword $(NIX_WRAPPED_GOALS))
  endif
endif

info-nix-env:
	@echo "NIX_ENV: $(NIX_ENV)"
	@echo "NIX_FLAKE: $(NIX_FLAKE)"
	@echo "NIX: $(NIX)"
	@echo "BUILD_IN_NIX: $(BUILD_IN_NIX)"
	@echo "INSIDE_NIX: $(INSIDE_NIX)"
	@echo "NIX_WRAP: $(NIX_WRAP)"

..in-nix-shell:
	@command -v $(NIX) >/dev/null 2>&1 || { \
		echo "BUILD_IN_NIX=1 but '$(NIX)' was not found in PATH."; \
		exit 1; \
	}
	@$(COLOR_ECHO) '$(COLOR_GREEN)Launching Nix shell "$(NIX_ENV)" for "$(if $(NIX_WRAPPED_GOALS),$(NIX_WRAPPED_GOALS),default goal)".$(COLOR_RESET)'
	@env MAKEFLAGS="$(subst ','\'',$(NIX_INNER_MAKEFLAGS))" \
		$(NIX) develop $(NIX_DEVELOP_FLAGS) '$(NIX_FLAKE)#$(NIX_ENV)' --command \
		$(MAKE) $(NIX_MAKE_JOBS) BUILD_IN_NIX=0 INSIDE_NIX=1 NIX_ENV='$(NIX_ENV)' \
		$(NIX_WRAPPED_GOALS)
