# Makefile to build the OTA image
include Makefile.conf

BOARDDIR = target/$(TARGET)/$(BOARD)

CONTIKI_PROJECT=runner
all: ota

#####################################################
#               Configurable Parameters             #
#####################################################
# Accepted values: 0 1 2 3
LOGGER_VERBOSITY ?= 4
# Accepted values: TINYDTLS, CRYPTOAUTHLIB, TINYCRYPT
SEC_LIB ?= TINYDTLS
# Accepted values: CONN_UDP, CONN_DTLS_PSK, CONN_DTLS_ECDSA
CONN_TYPE ?= CONN_UDP
#####################################################
#              End Configurable Parameters          #
#####################################################

# Contiki Size optimizations
SMALL = 1

# Include libpull sources
ROOTDIR=../..
LIBPULL += $(ROOTDIR)/src/network $(ROOTDIR)/src/common $(ROOTDIR)/src/memory 
LIBPULL += $(ROOTDIR)/src/security transport $(BOARDDIR)
LIBPULL_SOURCEFILES = $(notdir $(shell find $(LIBPULL) -maxdepth 1 \( -name '*.c' ! -name 'linker_script.c' \) ))
LIBPULL_INCLUDES = -I$(ROOTDIR)/include -Itransport
CFLAGS += -DSIMPLE_MANIFEST -DLOGGER_VERBOSITY=$(LOGGER_VERBOSITY)

# Add libpull to the build
PROJECTDIRS += $(LIBPULL)
PROJECT_SOURCEFILES += $(LIBPULL_SOURCEFILES)
CFLAGS += $(LIBPULL_INCLUDES)

# Include CryptoAuthLib for performing verification
EXT=ext
ifeq ($(SEC_LIB),TINYDTLS)
	# TinyDTLS is included in any case since it may be needed for the DTLS
	# connection. However, if is not used the compiler is enough smart to
	# remove the symbols
	CFLAGS += -DWITH_TINYDTLS=1
else ifeq ($(SEC_LIB),CRYPTOAUTHLIB)
	CFLAGS += -DWITH_CRYPTOAUTHLIB
	CAUTHLIB_DIR = $(EXT)/cryptoauthlib/lib/
	PROJECTDIRS += $(CAUTHLIB_DIR) $(CAUTHLIB_DIR)/basic $(CAUTHLIB_DIR)/host
	PROJECTDIRS += $(CAUTHLIB_DIR)/hal $(CAUTHLIB_DIR)/crypto $(CAUTHLIB_DIR)/crypto/hashes
	PROJECT_SOURCEFILES += $(notdir $(wildcard $(CAUTHLIB_DIR)/*.c $(CAUTHLIB_DIR)/basic/*.c $(CAUTHLIB_DIR)/host/*.  c $(CAUTHLIB_DIR)/crypto/hashes/*.c))
	PROJECT_SOURCEFILES += hal_cc2650_i2c.c atca_hal.c atca_crypto_sw_sha2.c
	CFLAGS+= -I$(CAUTHLIB_DIR) -I$(CAUTHLIB_DIR)/basic
	CFLAGS+= -I$(CAUTHLIB_DIR)/host -I$(CAUTHLIB_DIR)/hal -I$(CAUTHLIB_DIR)/crypto
	CFLAGS+=-DATCA_HAL_I2C
else ifeq ($(SEC_LIB),TINYCRYPT)
	CFLAGS += -DWITH_TINYCRYPT=1
	CFLAGS += -I$(EXT)/tinycrypt/lib/include
	PROJECTDIRS += $(EXT)/tinycrypt/lib/source
	PROJECT_SOURCEFILES += ecc_dsa.c sha256.c utils.c ecc.c
endif

# Include Tinydtls sources
PROJECTDIRS += ext/tinydtls ext/tinydtls/aes ext/tinydtls/sha2 ext/tinydtls/ecc
PROJECT_SOURCEFILES += dtls.c crypto.c hmac.c rijndael.c sha2.c
PROJECT_SOURCEFILES += ccm.c netq.c dtls_time.c peer.c session.c ecc.c
CFLAGS += -Iext/tinydtls -Iext/tinydtls/aes -Iext/tinydtls/sha2 -Iext/tinydtls/ecc
CFLAGS += -DDTLS_CONF_ECC=1 -DWITH_SHA256 -DSHA2_USE_INTTYPES_H
TINYDTLS_NDEBUG ?= 1
CFLAGS += -DDTLS_CONF_NDEBUG=$(TINYDTLS_NDEBUG)
ifeq ($(TINYDTLS_NDEBUG),0)
	TINYDTLS_SOURCEFILES += dtls_debug.c
else
	CFLAGS += -DNDEBUG=0
endif

# Configure the connection type
CFLAGS += -D$(CONN_TYPE)=1
ifeq ($(CONN_TYPE),CONN_UDP)
	# I do not need to set any parameter
else ifeq ($(CONN_TYPE),CONN_DTLS_ECDSA)
	CFLAGS += -DWITH_DTLS_COAP=1
	CFLAGS += -DDTLS_MAX_BUF=250
else ifeq ($(CONN_TYPE),CONN_DTLS_PSK)
	CFLAGS += -DWITH_DTLS_COAP=1
	CFLAGS += -DDTLS_CONF_PSK=1
endif

# Coap Modules
MODULES += os/net/app-layer/coap

# Runner Configuration
CFLAGS += -DOTA=1 -DPAGE_SIZE=$(PAGE_SIZE)
CFLAGS += -DBOOTLOADER_START_PAGE=$(BOOTLOADER_START_PAGE)
CFLAGS += -DBOOTLOADER_END_PAGE=$(BOOTLOADER_END_PAGE)
CFLAGS += -DIMAGE_START_PAGE=$(IMAGE_START_PAGE)
CFLAGS += -DIMAGE_END_PAGE=$(IMAGE_END_PAGE)
CFLAGS += -DMANIFEST_SIZE=$(MANIFEST_SIZE)
CFLAGS += -DBOOTLOADER_CTX_START_OFFSET=$(BOOTLOADER_CTX_START_OFFSET)
CFLAGS += -DBOOTLOADER_CTX_END_OFFSET=$(BOOTLOADER_CTX_END_OFFSET)
CFLAGS += -DRECOVERY_IMAGE=$(RECOVERY_IMAGE)

OTA_OFFSET="($(IMAGE_START_PAGE)*$(PAGE_SIZE))+$(MANIFEST_SIZE)"
OTA_LENGTH="(($(IMAGE_END_PAGE)-$(IMAGE_START_PAGE))*$(PAGE_SIZE)-$(MANIFEST_SIZE))"
LINKER_DEFINES=-DOTA_OFFSET=$(OTA_OFFSET) -DOTA_LENGTH=$(OTA_LENGTH) -DCCFG_DISABLE=1

ota:
	$(Q)$(LD) -P -E $(LINKER_DEFINES) $(BOARDDIR)/linker_script.c -o linker_script.ld
	$(MAKE) $(CONTIKI_PROJECT) LDSCRIPT=linker_script.ld

# Include Contiki Makefile
CONTIKI = ext/contiki-ng
include $(CONTIKI)/Makefile.include
