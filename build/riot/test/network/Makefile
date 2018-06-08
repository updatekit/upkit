#####################################################
#                   TEST NETWORK                    #
# This Makefile is used to build the network test   #
# for the RIOT platform.                            #
#####################################################
APPLICATION = test_network
BOARD ?= remote-reva

# Build configuration
DEVELHELP ?= 1
QUIET ?= 1
WERROR=0

# Define libpull dirs
ROOTDIR = $(CURDIR)/../../../..
BOARDDIR = $(CURDIR)/../../boards/$(BOARD)
CONNECTIONDIR = $(CURDIR)/../../connection

TEST_PLATFORM = $(ROOTDIR)/test/platform
TEST_UNITY = $(ROOTDIR)/ext/unity/src

# libpull configuration
LOGGER_VERBOSITY = 3

CFLAGS += -DSIMPLE_MANIFEST -DLOGGER_VERBOSITY=$(LOGGER_VERBOSITY)
CFLAGS += -DMEMORY_FLASH_IMPL=1

# Includes directories
INCLUDES += -I$(ROOTDIR)/include -I$(ROOTDIR) -I$(CONNECTIONDIR)
INCLUDES += -I$(BOARDDIR) -I$(TEST_PLATFORM) -I$(TEST_UNITY)

# Include libpull modules
EXTERNAL_MODULE_DIRS = $(BOARDDIR) $(ROOTDIR)/src $(CONNECTIONDIR) $(TEST_UNITY)
USEMODULE += board_remote_reva riot_connection libpull unity

# Include libcoap
USEPKG += libcoap
INCLUDES += -I$(CURDIR)/bin/pkg/remote-reva/libcoap/include

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_sock_udp

# Use setsockopt to set a timeout
CFLAGS += -DPOSIX_SETSOCKOPT=1

# Add shell support
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Include the RIOT Makfile
RIOTBASE ?= $(CURDIR)/../../ext/riot
include $(RIOTBASE)/Makefile.include
