AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = -Wall -I. -Iresources -I../../include -I../../ext/tinycrypt/lib/include -I../../ext/tinydtls -I../../ext/libcoap/include -DUSE_TESTING_KEYS=1

LIBS = ../../libpull.a ../../ext/libcoap/.libs/libcoap-1.a ../../ext/tinydtls/libtinydtls.a
LIBS += ../../ext/tinycrypt/lib/libtinycrypt.a

bin_PROGRAMS = server

server_SOURCES = server.c network.c
server_SOURCES += resources/echo.c resources/firmware.c resources/firmware_invalid_signature.c
server_SOURCES += resources/firmware_invalid_size.c resources/resources.h resources/version.c
server_SOURCES += resources/version_invalid.c resources/next_version.c
server_SOURCES += ../../ext/tinycrypt/lib/source/ecc_platform_specific.c
