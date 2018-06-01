#####################################################
# 				Configurable Parameters             #
#####################################################
# Accepted values: 0 1 2 3 4
LOGGER_VERBOSITY ?= 4
# Accepted values: NULL, TINYDTLS, CRYPTOAUTHLIB, TINYCRYPT
SEC_LIB ?= TINYCRYPT
#####################################################
#   		   End Configurable Parameters          #
#####################################################
include ../Makefile.conf

CONTIKI_PROJECT=bootloader
all: ota

# Contiki-NG Size optimizations
SMALL = 1
MAKE_MAC = MAKE_MAC_NULLMAC
MAKE_NET = MAKE_NET_NULLNET

WERROR=0

# Add libpull sources
ROOTDIR = ../../..
BOARDDIR = ../target/$(TARGET)/$(BOARD)

# Bootloader Configurations
LIBPULL_BOOTLOADER=1
DISABLE_MAIN=0
include ../makefiles/Makefile.libpull

CFLAGS += -DDISABLE_ALL_OUTPUT=1

# Security Configurations
ext= ../ext
ifeq ($(SEC_LIB),TINYDTLS)
	include ../makefiles/Makefile.tinydtls
else ifeq ($(SEC_LIB),CRYPTOAUTHLIB)
	include ../makefiles/Makefile.cryptoauthlib
else ifeq ($(SEC_LIB),TINYCRYPT)
	include ../makefiles/Makefile.tinycrypt
endif

CFLAGS += -DPAGE_SIZE=$(PAGE_SIZE)
CFLAGS += -DBOOTLOADER_START_PAGE=$(BOOTLOADER_START_PAGE)
CFLAGS += -DBOOTLOADER_END_PAGE=$(BOOTLOADER_END_PAGE)
CFLAGS += -DIMAGE_START_PAGE=$(IMAGE_START_PAGE)
CFLAGS += -DIMAGE_END_PAGE=$(IMAGE_END_PAGE)
CFLAGS += -DMANIFEST_SIZE=$(MANIFEST_SIZE)
CFLAGS += -DBOOTLOADER_CTX_START_OFFSET=$(BOOTLOADER_CTX_START_OFFSET)
CFLAGS += -DBOOTLOADER_CTX_END_OFFSET=$(BOOTLOADER_CTX_END_OFFSET)
CFLAGS += -DRECOVERY_IMAGE=$(RECOVERY_IMAGE)
CFLAGS += -DINITIAL_MEMORY_OFFSET=$(INITIAL_MEMORY_OFFSET)

CFLAGS += -DSEC_LIB=$(SEC_LIB)

OTA_OFFSET="$(INITIAL_MEMORY_OFFSET)+($(BOOTLOADER_START_PAGE)*$(PAGE_SIZE))"
OTA_LENGTH="($(BOOTLOADER_END_PAGE)-$(BOOTLOADER_START_PAGE))*$(PAGE_SIZE)"
CFLAGS += -DOTA_OFFSET=$(OTA_OFFSET) -DOTA_LENGTH=$(OTA_LENGTH)

# The bootloader_ctx must be compiled for x86, so use direcly gcc
ctx:
	gcc -Wall bootloader_ctx.c -I$(ROOTDIR)/include -o bootloader_ctx
	./bootloader_ctx bootloader_ctx.bin && echo "Bootloader ctx generated"

ota:
	#$(Q)$(LD) -P -E $(LINKER_DEFINES) $(BOARDDIR)/linker_script.c -o linker_script.ld
	$(MAKE) $(CONTIKI_PROJECT) SOURCE_LDSCRIPT=$(BOARDDIR)/linker_script.c
	$(MAKE) ctx

# Include Contiki Makefile
CONTIKI = ../ext/contiki-ng
include $(CONTIKI)/Makefile.include
