#####################################################
#               Configurable Parameters             #
#####################################################
# Accepted values: 0 1 2 3 4
LOGGER_VERBOSITY ?= 4
# Accepted values: NULL, TINYDTLS, CRYPTOAUTHLIB, TINYCRYPT
SEC_LIB ?= TINYDTLS
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

export ENABLE_BOOTLOADER = 1

EXTERNAL_MODULE_DIRS = $(BOARDDIR) $(ROOTDIR)/src $(ROOTDIR)/agents
USEMODULE += board_remote_reva libpull agents

CFLAGS += -I$(ROOTDIR)/include -I$(ROOTDIR) -I$(BOARDDIR)
CFLAGS += -DSIMPLE_MANIFEST -DLOGGER_VERBOSITY=$(LOGGER_VERBOSITY)
# Enable the flash interface
CFLAGS += -DMEMORY_FLASH_IMPL=1 -DDISABLE_LIBPULL_NETWORK=1

CFLAGS += -DWITH_TINYCRYPT

# Add tinycrypt to the build
USEPKG += tinycrypt

bootloader_ctx:
	gcc -Wall bootloader_ctx.c -I$(ROOTDIR)/include -o bootloader_ctx
	./bootloader_ctx bootloader_ctx.bin && echo "Bootloader ctx generated"

include $(RIOTBASE)/Makefile.include
