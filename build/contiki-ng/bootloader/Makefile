# Makefile for the Bootloader
ifndef TARGET
	TARGET=srf06-cc26xx
	BOARD=sensortag/cc2650
endif

BOARDDIR = ../target/$(TARGET)/$(BOARD)

CONTIKI_PROJECT=bootloader
all: ota

# Default configs
CFLAGS += -DSIMPLE_MANIFEST -DLOGGER_VERBOSITY=3

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

# Size optimizations
SMALL = 1
MAKE_MAC = MAKE_MAC_NULLMAC
MAKE_NET = MAKE_NET_NULLNET

# Security Configurations
# The bootloader needs to use a cryptographic library to verify the digital
# signature of the firmware before booting. Actually it supports Tinydtls
# and CryptoAuthLib, the library to interact with the Atmel CryptoAuthentication
# device. Select the library you want to include into the build.
# Accepted values: TINYDTLS, CRYPTOAUTHLIB
SEC_LIB ?= CRYPTOAUTHLIB
ext= ../ext
ifeq ($(SEC_LIB),TINYDTLS)
	CFLAGS += -DWITH_TINYDTLS
	# Add Tinydtls sources
	PROJECTDIRS += $(ext)/tinydtls $(ext)/tinydtls/aes $(ext)/tinydtls/sha2 $(ext)/tinydtls/ecc
	PROJECT_SOURCEFILES += dtls.c crypto.c hmac.c rijndael.c sha2.c
	PROJECT_SOURCEFILES += ccm.c netq.c dtls_time.c peer.c session.c ecc.c
	CFLAGS += -I$(ext)/tinydtls -I$(ext)/tinydtls/aes -I$(ext)/tinydtls/sha2 -I$(ext)/tinydtls/ecc
	ifeq ($(DTLS_DEBUG),1)
		TINYDTLS_SOURCEFILES += dtls_debug.c
	else
		CFLAGS += -DNDEBUG=0
	endif
	# Tinydtls configurations
	CFLAGS += -DDTLS_CONF_ECC=1
	CFLAGS += -DWITH_SHA256 -DSHA2_USE_INTTYPES_H
else ifeq ($(SEC_LIB),CRYPTOAUTHLIB)
	CFLAGS += -DWITH_CRYPTOAUTHLIB
	CAUTHLIB_DIR = $(ext)/cryptoauthlib/lib/
	PROJECTDIRS += $(CAUTHLIB_DIR) $(CAUTHLIB_DIR)/basic $(CAUTHLIB_DIR)/host 
	PROJECTDIRS += $(CAUTHLIB_DIR)/hal $(CAUTHLIB_DIR)/crypto $(CAUTHLIB_DIR)/crypto/hashes
	PROJECT_SOURCEFILES += $(notdir $(wildcard $(CAUTHLIB_DIR)/*.c $(CAUTHLIB_DIR)/basic/*.c $(CAUTHLIB_DIR)/host/*.c $(CAUTHLIB_DIR)/crypto/hashes/*.c))
	PROJECT_SOURCEFILES += hal_cc2650_i2c.c atca_hal.c atca_crypto_sw_sha2.c
	CFLAGS+= -I$(CAUTHLIB_DIR) -I$(CAUTHLIB_DIR)/basic 
	CFLAGS+= -I$(CAUTHLIB_DIR)/host -I$(CAUTHLIB_DIR)/hal -I$(CAUTHLIB_DIR)/crypto
	CFLAGS+=-DATCA_HAL_I2C
endif

# Bootloader configuration
DISABLE_MAIN=1
OTA_OFFSET=0x0
PAGES=6
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
