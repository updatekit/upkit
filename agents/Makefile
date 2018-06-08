# This makefile is required to build the library for RIOT as a module.
MODULE = agents

NO_AUTO_SRC=1

ifeq ($(ENABLE_BOOTLOADER), 1)
	SRC += bootloader_agent.c
endif
ifeq ($(ENABLE_UPDATE_AGENT), 1)
	SRC += update.c
endif

CFLAGS += -I../include -DMEMORY_FLASH_IMPL=1 -DSIMPLE_MANIFEST=1

include $(RIOTBASE)/Makefile.base
