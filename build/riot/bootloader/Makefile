#####################################################
#               Configurable Parameters             #
#####################################################
# Accepted values: 0 1 2 3 4
LOGGER_VERBOSITY ?= 0
# Accepted values: NULL, TINYDTLS, TINYCRYPT
SEC_LIB ?= TINYCRYPT
#####################################################
#              End Configurable Parameters          #
#####################################################

APPLICATION = bootloader
BOARD ?= remote-reva

RIOTBASE ?= $(CURDIR)/../ext/riot

DEVELHELP ?= 1
QUIET ?= 1
WERROR=0

# Add libpull to the build
ROOTDIR = $(CURDIR)/../../..
BOARDDIR = $(CURDIR)/../boards/$(BOARD)

include $(BOARDDIR)/Makefile.conf
include $(BOARDDIR)/Makefile.export
export ENABLE_BOOTLOADER = 1

CFLAGS += $(BOARD_CFLAGS)

EXTERNAL_MODULE_DIRS = $(ROOTDIR)/src
USEMODULE += libpull

ifeq ($(BOARD),remote-reva)
	CFLAGS += -I$(ROOTDIR)/src/platform/cc2538
	CFLAGS += -DWITH_PLATFORM_CC2538=1
	CFLAGS += -DWITH_ARM_CORTEX_LOADER=1
endif

CFLAGS += -I$(ROOTDIR)/include -I$(ROOTDIR)
CFLAGS += -DSIMPLE_MANIFEST -DLOGGER_VERBOSITY=$(LOGGER_VERBOSITY)
# Enable the flash interface
CFLAGS += -DMEMORY_FLASH_IMPL=1 -DDISABLE_LIBPULL_NETWORK=1

ifeq ($(SEC_LIB),TINYCRYPT) 
	# Add tinycrypt to the build
	CFLAGS += -DWITH_TINYCRYPT=1
	USEPKG += tinycrypt
else ifeq ($(SEC_LIB),TINYDTLS)
	# Add tinydtls to the build
	CFLAGS += -DWITH_TINYDTLS=1
	USEPKG += tinydtls
	CFLAGS += -DDTLS_ECC -DWITH_SHA256 -DWITH_RIOT_GNRC
endif

bootloader_ctx:
	gcc -Wall bootloader_ctx.c -I$(ROOTDIR)/include -o bootloader_ctx
	./bootloader_ctx bootloader_ctx.bin && echo "Bootloader ctx generated"

include $(RIOTBASE)/Makefile.include
