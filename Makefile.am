AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = test

AM_CFLAGS = -Wall -I. -Iinclude -Iext/tinycrypt/lib/include -Iext/tinydtls -Iext/mbedtls/include -DHAVE_CONFIG_H -ffreestanding -pedantic-errors -std=c99 -DENABLE_SIGN=1 -DSIMPLE_MANIFEST=1 -DWITH_TINYDTLS=1 -DWITH_TINYCRYPT=1 -DWITH_MBEDTLS=1

noinst_LIBRARIES = libpull.a

libpull_a_SOURCES = src/core/common/error.c
libpull_a_SOURCES += src/core/memory/manifest.c src/core/memory/memory_objects.c src/core/memory/simple_manifest.c
libpull_a_SOURCES += src/core/memory/memory_impl.c src/core/memory/memory_flash.c
libpull_a_SOURCES += src/core/network/connection_config.c src/core/network/fsm.c src/core/network/request.c
libpull_a_SOURCES += src/core/security/verifier.c src/core/security/tinycrypt.c src/core/security/tinydtls.c src/core/security/cryptoauthlib.c src/core/security/safestore.c src/core/security/mbedtls.c
libpull_a_SOURCES += src/core/pipeline/lzss.c src/core/pipeline/bspatch.c src/core/pipeline/buffer.c src/core/pipeline/writer.c

pkginclude_HEADERS = include/
# TODO Finish including all the headers

assets: firmware_tool
	./utils/assets_generator.sh

firmware_tool: libpull.a
	$(MAKE) -C utils/firmware_tool

run_server: server assets
	./utils/server/server -f assets/slot_d.bin

server: libpull.a
	$(MAKE) -C utils/server
