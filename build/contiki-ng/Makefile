# Target Platform
ifndef TARGET
	TARGET=srf06-cc26xx
	BOARD=sensortag/cc2650
endif
BOARDDIR = target/$(TARGET)/$(BOARD)

CONTIKI_PROJECT=runner
all: ota

# Size optimizations
SMALL = 1

# Include libpull sources
ROOTDIR=../..
LIBPULL += $(ROOTDIR)/src/network $(ROOTDIR)/src/common $(ROOTDIR)/src/memory 
LIBPULL += $(ROOTDIR)/src/security transport $(BOARDDIR)
LIBPULL_SOURCEFILES = $(notdir $(shell find $(LIBPULL) -maxdepth 1 \( -name '*.c' ! -name 'linker_script.c' \) ))
LIBPULL_INCLUDES = -I$(ROOTDIR)/include -Itransport

# Add libpull to the build
PROJECTDIRS += $(LIBPULL)
PROJECT_SOURCEFILES += $(LIBPULL_SOURCEFILES)
CFLAGS += $(LIBPULL_INCLUDES)
CFLAGS += -DWITH_TINYDTLS -DLOGGER_VERBOSITY=4

# Include Tinydtls sources
TINYDTLS += ext/tinydtls ext/tinydtls/aes ext/tinydtls/sha2 ext/tinydtls/ecc
TINYDTLS_SOURCEFILES += dtls.c crypto.c hmac.c rijndael.c sha2.c
TINYDTLS_SOURCEFILES += ccm.c netq.c dtls_time.c peer.c session.c ecc.c
TINYDTLS_INCLUDES += -Iext/tinydtls -Iext/tinydtls/aes -Iext/tinydtls/sha2 -Iext/tinydtls/ecc
CFLAGS+= -DDTLS_CONF_NDEBUG=1
ifeq ($(DTLS_DEBUG),1)
	TINYDTLS_SOURCEFILES += dtls_debug.c
else
	CFLAGS += -DNDEBUG=0
endif
# Add Tinydtls to the build
PROJECTDIRS += $(TINYDTLS)
PROJECT_SOURCEFILES += $(TINYDTLS_SOURCEFILES)
CFLAGS += $(TINYDTLS_INCLUDES)
CFLAGS += -DDTLS_CONF_ECC=1 -DWITH_SHA256 -DSHA2_USE_INTTYPES_H

# CONN_TYPE can be UDP, DTLS_PSK, DTLS_ECDSA
CONN_TYPE=CONN_DTLS_ECDSA
CFLAGS += -D$(CONN_TYPE)=1
ifeq ($(CONN_TYPE),CONN_UDP)
	# I do not need to set any parameter
else
	ifeq ($(CONN_TYPE),CONN_DTLS_ECDSA)
		CFLAGS += -DDTLS_ECC_CONN=1
		CFLAGS += -DWITH_DTLS_COAP=1
		CFLAGS += -DDTLS_CONF_PSK=0
		CFLAGS += -DDTLS_MAX_BUF=250
	else ifeq ($(CONN_TYPE),CONN_DTLS_PSK)
		CFLAGS += -DWITH_DTLS_COAP=1
		CFLAGS += -DDTLS_CONF_PSK=1
	endif
endif

# Coap Modules
MODULES += os/net/app-layer/coap

# OTA flags
CFLAGS += -DOTA=1
OTA_OFFSET=0x5100
PAGES=26
OTA_LENGTH=0x1000*$(PAGES)
LINKER_DEFINES=-DOTA_OFFSET=$(OTA_OFFSET) -DOTA_LENGTH=$(OTA_LENGTH) -DCCFG_DISABLE=1
ota:
	$(Q)$(LD) -P -E $(LINKER_DEFINES) $(BOARDDIR)/linker_script.c -o linker_script.ld
	$(MAKE) $(CONTIKI_PROJECT) LDSCRIPT=linker_script.ld

# Include Contiki Makefile
CONTIKI = ext/contiki-ng
include $(CONTIKI)/Makefile.include
