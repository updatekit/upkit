# Makefile for the Bootloader
ifndef TARGET
	TARGET=srf06-cc26xx
	BOARD=sensortag/cc2650
endif
BOARDDIR = ../target/$(TARGET)/$(BOARD)

CONTIKI_PROJECT=bootloader
all: ota

# Add libpull sources
ROOTDIR=../../..
LIBPULL = $(ROOTDIR)/src/common $(ROOTDIR)/src/memory $(ROOTDIR)/src/security
LIBPULL += $(BOARDDIR) $(BOARDDIR)/bootloader
LIBPULL_SOURCEFILES = $(notdir $(shell find $(LIBPULL) -maxdepth 1 \( -name '*.c' ! -name 'linker_script.c' \) ))
LIBPULL_INCLUDES = -I$(ROOTDIR)/include

# Include libpull sources
PROJECTDIRS = $(LIBPULL)
PROJECT_SOURCEFILES = $(LIBPULL_SOURCEFILES)
CFLAGS += $(LIBPULL_INCLUDES)
CFLAGS += -DLOGGER_VERBOSITY=4 -DWITH_TINYDTLS

# Size optimizations
SMALL = 1
MAKE_MAC = MAKE_MAC_NULLMAC
MAKE_NET = MAKE_NET_NULLNET

# Add Tinydtls sources
ext= ../ext
TINYDTLS += $(ext)/tinydtls $(ext)/tinydtls/aes $(ext)/tinydtls/sha2 $(ext)/tinydtls/ecc
TINYDTLS_SOURCEFILES += dtls.c crypto.c hmac.c rijndael.c sha2.c
TINYDTLS_SOURCEFILES += ccm.c netq.c dtls_time.c peer.c session.c ecc.c
TINYDTLS_INCLUDES += -I$(ext)/tinydtls -I$(ext)/tinydtls/aes -I$(ext)/tinydtls/sha2 -I$(ext)/tinydtls/ecc
ifeq ($(DTLS_DEBUG),1)
	TINYDTLS_SOURCEFILES += dtls_debug.c
else
	CFLAGS += -DNDEBUG=0
endif

# Include Tinydtls into the build
PROJECTDIRS += $(TINYDTLS)
PROJECT_SOURCEFILES += $(TINYDTLS_SOURCEFILES)
CFLAGS += $(TINYDTLS_INCLUDES)
CFLAGS += -DDTLS_CONF_ECC=1
CFLAGS += -DWITH_SHA256 -DSHA2_USE_INTTYPES_H

# Bootloader configuration
DISABLE_MAIN=1
OTA_OFFSET=0x0
PAGES=5
OTA_LENGTH=0x1000*$(PAGES)
LINKER_DEFINES=-DOTA_OFFSET=$(OTA_OFFSET) -DOTA_LENGTH=$(OTA_LENGTH)
# The bootloader_ctx must be compiled for x86, so use direcly gcc
ctx:
	gcc -Wall bootloader_ctx.c -o bootloader_ctx
	./bootloader_ctx bootloader_ctx.bin && echo "Bootloader ctx generated"

ota:
	$(Q)$(LD) -P -E $(LINKER_DEFINES) $(BOARDDIR)/linker_script.c -o linker_script.ld
	$(MAKE) $(CONTIKI_PROJECT) LDSCRIPT=linker_script.ld
	$(MAKE) ctx

# Include Contiki Makefile
CONTIKI = ../ext/contiki-ng
include $(CONTIKI)/Makefile.include
