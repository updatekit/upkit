#####################################################
#               Configurable Parameters             #
#####################################################
# Accepted values: 0 1 2 3 4
LOGGER_VERBOSITY ?= 3
# Accepted values: NULL, TINYDTLS, CRYPTOAUTHLIB, TINYCRYPT
SEC_LIB ?= TINYCRYPT
# Accepted values: CONN_UDP, CONN_DTLS_PSK
CONN_TYPE ?= CONN_UDP
#####################################################
#              End Configurable Parameters          #
#####################################################
include Makefile.target
include target/$(TARGET)/$(BOARD)/Makefile.conf

CONTIKI_PROJECT=runner
all: ota

WERROR = 0

# Contiki Size optimizations
SMALL = 1

# Define dirs
ROOTDIR=../..
BOARDDIR = target/$(TARGET)/$(BOARD)
LIBPULL_APPLICATION=1
include makefiles/Makefile.libpull

# Add the update process
PROJECT_SOURCEFILES += update_process.c

# Include CryptoAuthLib for performing verification
ext=ext
ifeq ($(SEC_LIB),TINYDTLS)
	include makefiles/Makefile.tinydtls
else ifeq ($(SEC_LIB),CRYPTOAUTHLIB)
	include makefiles/Makefile.cryptoauthlib
else ifeq ($(SEC_LIB),TINYCRYPT)
	include makefiles/Makefile.tinycrypt
endif

# Configure the connection type
CFLAGS += -D$(CONN_TYPE)=1
ifeq ($(CONN_TYPE),CONN_UDP)
	# I do not need to set any parameter
else ifeq ($(CONN_TYPE),CONN_DTLS_PSK)
	MAKE_WITH_DTLS=1
	MAKE_COAP_DTLS_KEYSTORE=MAKE_COAP_DTLS_KEYSTORE_SIMPLE
	CFLAGS+= -DSHA2_USE_INTTYPES_H=1 -DWITH_SHA256=1
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
CFLAGS += -DINITIAL_MEMORY_OFFSET=$(INITIAL_MEMORY_OFFSET)

CFLAGS += -DCONN_TYPE=$(CONN_TYPE)
CFLAGS += -DSEC_LIB=$(SEC_LIB)

OTA_OFFSET="$(INITIAL_MEMORY_OFFSET)+($(IMAGE_START_PAGE)*$(PAGE_SIZE))+$(MANIFEST_SIZE)"
OTA_LENGTH="(($(IMAGE_END_PAGE)-$(IMAGE_START_PAGE))*$(PAGE_SIZE)-$(MANIFEST_SIZE))"
CFLAGS += -DOTA_OFFSET=$(OTA_OFFSET) -DOTA_LENGTH=$(OTA_LENGTH) -DCCFG_DISABLE=1

ota:
	#$(Q)$(LD) -P -E $(LINKER_DEFINES) $(BOARDDIR)/linker_script.c -o $(OBJECTDIR)/cc2538.ld
	$(MAKE) $(CONTIKI_PROJECT) LDSCRIPT=$(OBJECTDIR)/cc2538.ld SOURCE_LDSCRIPT=$(BOARDDIR)/linker_script.c

# Include Contiki Makefile
CONTIKI = ext/contiki-ng
include $(CONTIKI)/Makefile.include
