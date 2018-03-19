# Makefile for the Bootloader
include ../Makefile.conf

BOARDDIR = ../target/$(TARGET)/$(BOARD)

CONTIKI_PROJECT=bootloader
all: ota

#####################################################
# 				Configurable Parameters             #
#####################################################
# Accepted values: 0 1 2 3 
LOGGER_VERBOSITY ?= 0
# Accepted values: TINYDTLS, CRYPTOAUTHLIB, TINYCRYPT
SEC_LIB ?= TINYCRYPT
# Evaluate Time
EVALUATE_TIME=1
#####################################################
#   		   End Configurable Parameters          #
#####################################################


WERROR=0
# Add libpull sources
ROOTDIR=../../..
LIBPULL = $(ROOTDIR)/src/common $(ROOTDIR)/src/memory $(ROOTDIR)/src/security
LIBPULL += $(BOARDDIR) $(BOARDDIR)/bootloader
LIBPULL_SOURCEFILES = $(notdir $(shell find $(LIBPULL) -maxdepth 1 \( -name '*.c' ! -name 'linker_script.c' \) ))
LIBPULL_INCLUDES = -I$(ROOTDIR)/include -I..
CFLAGS += -DSIMPLE_MANIFEST -DLOGGER_VERBOSITY=$(LOGGER_VERBOSITY)

# Include libpull sources
PROJECTDIRS = $(LIBPULL)
PROJECT_SOURCEFILES = $(LIBPULL_SOURCEFILES)
CFLAGS += $(LIBPULL_INCLUDES)

# Contiki-NG Size optimizations
SMALL = 1
MAKE_MAC = MAKE_MAC_NULLMAC
MAKE_NET = MAKE_NET_NULLNET

# Security Configurations
ext= ../ext
ifeq ($(SEC_LIB),TINYDTLS)
	CFLAGS += -DWITH_TINYDTLS=1
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
else ifeq ($(SEC_LIB),TINYCRYPT)
	CFLAGS += -DWITH_TINYCRYPT=1
	CFLAGS += -I$(ext)/tinycrypt/lib/include
	PROJECTDIRS += $(ext)/tinycrypt/lib/source
	PROJECT_SOURCEFILES += ecc_dsa.c sha256.c utils.c ecc.c
endif

# Bootloader Configurations
DISABLE_MAIN=1
CFLAGS += -DPAGE_SIZE=$(PAGE_SIZE)
CFLAGS += -DBOOTLOADER_START_PAGE=$(BOOTLOADER_START_PAGE)
CFLAGS += -DBOOTLOADER_END_PAGE=$(BOOTLOADER_END_PAGE)
CFLAGS += -DIMAGE_START_PAGE=$(IMAGE_START_PAGE)
CFLAGS += -DIMAGE_END_PAGE=$(IMAGE_END_PAGE)
CFLAGS += -DMANIFEST_SIZE=$(MANIFEST_SIZE)
CFLAGS += -DBOOTLOADER_CTX_START_OFFSET=$(BOOTLOADER_CTX_START_OFFSET)
CFLAGS += -DBOOTLOADER_CTX_END_OFFSET=$(BOOTLOADER_CTX_END_OFFSET)
CFLAGS += -DRECOVERY_IMAGE=$(RECOVERY_IMAGE)


CFLAGS += -DEVALUATE_TIME=$(EVALUATE_TIME)
CFLAGS += -DSEC_LIB=$(SEC_LIB)
CFLAGS += -DSEC_LIB_STR="\"$(SEC_LIB)\""

OTA_OFFSET="$(BOOTLOADER_START_PAGE)*$(PAGE_SIZE)"
OTA_LENGTH="($(BOOTLOADER_END_PAGE)-$(BOOTLOADER_START_PAGE))*$(PAGE_SIZE)"
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
