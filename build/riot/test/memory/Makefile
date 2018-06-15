#####################################################
#                   TEST MEMORY                     #
# This Makefile is used to build the memory test    #
# for the RIOT platform.                            #
#####################################################
APPLICATION = test_memory
BOARD ?= remote-reva

# Build configuration
DEVELHELP ?= 1
QUIET ?= 1
WERROR=0

# Define libpull dirs
ROOTDIR = $(CURDIR)/../../../..
BOARDDIR = $(CURDIR)/../../boards/$(BOARD)

TEST_PLATFORM = $(ROOTDIR)/test/platform
TEST_UNITY = $(ROOTDIR)/ext/unity/src

# libpull configuration
LOGGER_VERBOSITY = 3

CFLAGS += -DSIMPLE_MANIFEST -DLOGGER_VERBOSITY=$(LOGGER_VERBOSITY)
CFLAGS += -DMEMORY_FLASH_IMPL=1 -DDISABLE_LIBPULL_NETWORK=1

# Includes directories
INCLUDES += -I$(ROOTDIR)/include -I$(ROOTDIR) 
INCLUDES += -I$(BOARDDIR) -I$(TEST_PLATFORM) -I$(TEST_UNITY)

# Include libpull modules
EXTERNAL_MODULE_DIRS = $(BOARDDIR) $(ROOTDIR)/src $(TEST_UNITY)
USEMODULE += board_remote_reva libpull unity

# Include the RIOT Makfile
RIOTBASE ?= $(CURDIR)/../../ext/riot
include $(RIOTBASE)/Makefile.include
