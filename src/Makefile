# This makefile is required to build the libpull sources as a RIOT module
MODULE = libpull

# Enable Submodules
SUBMODULES := 1

LIBPULL_SUBMODULES += $(shell find . -type d ! -name .deps)

# Unless I find a cleaner way to tell the RIOT Makefile to build all the
# submodules without adding a Makefile in each folder, I have to create the
# subfolders manually for the libpull module
ifneq (0,$(foreach M, $(LIBPULL_SUBMODULES), $(shell (mkdir -p $(BINDIR)/$(MODULE)/$(M) 2>/dev/null); echo;)))
	SRC := $(foreach dir, $(LIBPULL_SUBMODULES), $(wildcard $(dir)/*.c))
endif

CFLAGS += -I../include -DMEMORY_FLASH_IMPL=1 -DSIMPLE_MANIFEST=1

# don't fail if requested submodule does not have corresponding *.c file
# SUBMODULES_NOFORCE := 1

include $(RIOTBASE)/Makefile.base
