#####################################################
#               Configurable Parameters             #
#####################################################
# Accepted values: 0 1 2 3 4
LOGGER_VERBOSITY ?= 4
# Accepted values: NULL, TINYDTLS, CRYPTOAUTHLIB, TINYCRYPT
SEC_LIB ?= TINYCRYPT
# Accepted values: CONN_UDP, CONN_DTLS_PSK
CONN_TYPE ?= CONN_UDP
#####################################################
#              End Configurable Parameters          #
#####################################################

APPLICATION = runner
BOARD ?= remote-reva

RIOTBASE ?= $(CURDIR)/../ext/riot
DEVELHELP ?= 1
WERROR = 0
QUIET ?= 1

# Include the libpull module to the build
ROOTDIR= $(CURDIR)/../../..
BOARDDIR = $(CURDIR)/../boards/$(BOARD)

#ifeq ($(BOARD),"remote-reva")
	CFLAGS += -I$(ROOTDIR)/src/platform/cc2538
	CFLAGS += -DWITH_PLATFORM_CC2538=1
#endif
CFLAGS += -DWITH_CONNECTION_LIBCOAP_RIOT=1
CFLAGS += -DWITH_UPDATE_AGENT

INCLUDES += -I$(ROOTDIR)/include -I$(ROOTDIR)
INCLUDES += -DSIMPLE_MANIFEST=1 -DLOGGER_VERBOSITY=$(LOGGER_VERBOSITY)
# Enable the flash interface
INCLUDES += -DMEMORY_FLASH_IMPL=1

ifeq ($(SEC_LIB),TINYDTLS)
	CFLAGS += -DWITH_TINYDTLS=1
	USEPKG += tinydtls
	USEMODULE += tinydtls_ecc
	USEMODULE += tinydtls_sha2
	CFLAGS += -DWITH_RIOT_SOCKETS=1
	CFLAGS += -DDTLSv12 -DWITH_SHA256
	CFLAGS += -DDTLS_ECC
else ifeq ($(SEC_LIB),TINYCRYPT)
	CFLAGS += -DWITH_TINYCRYPT=1
	USEPKG += tinycrypt
endif

EXTERNAL_MODULE_DIRS = $(ROOTDIR)/src $(BOARDDIR)
USEMODULE += libpull 

include $(BOARDDIR)/Makefile.conf
include $(BOARDDIR)/Makefile.export
CFLAGS += $(BOARD_CFLAGS)
export ENABLE_UPDATE_AGENT=1

# Allow to disable the external linker script to run the 
# image directly without bootloader
ifneq ($(NOOTA),1)
	export LINKER_SCRIPT=$(BINDIR)/linker_script.ld
endif

# A better way to disable the CCA is by defining
# UPDATE_CCA to 0
# -> boards/common/remote/include/board_common.h

# Add libcoap
USEPKG += libcoap
INCLUDES += -I$(CURDIR)/bin/pkg/remote-reva/libcoap/include
# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules
USEMODULE += gnrc_ipv6_default
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_sock_udp
CFLAGS += -DPOSIX_SETSOCKOPT=1

include $(RIOTBASE)/Makefile.include
