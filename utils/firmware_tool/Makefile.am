AUTOMAKE_OPTIONS = subdir-objects

AM_CXXFLAGS = -Wall -std=c++11 -I. -Icommands -I../../ext/tinycrypt/lib/include -I../../include
AM_CXXFLAGS += -I../../ext/tinydtls -Ideps -DWITH_TINYCRYPT=1

LIBS=../../libpull.a ../../ext/tinycrypt/lib/libtinycrypt.a

bin_PROGRAMS = firmware_tool

firmware_tool_SOURCES = firmware_tool.cpp context.cpp
firmware_tool_SOURCES += commands/configs.cpp commands/keys.cpp commands/help.cpp 
firmware_tool_SOURCES += commands/manifest.cpp commands/utils.cpp
firmware_tool_SOURCES += ../../ext/tinycrypt/lib/source/ecc_platform_specific.c
