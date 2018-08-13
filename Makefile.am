AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = test

AM_CFLAGS = -Wall -I. -Iinclude -Iext/tinycrypt/lib/include -Iext/tinydtls -DHAVE_CONFIG_H -ffreestanding -pedantic-errors

noinst_LIBRARIES = libpull.a

libpull_a_SOURCES = src/core/common/error.c src/core/common/identity.c src/core/common/crc16.c
libpull_a_SOURCES += src/core/memory/manifest.c src/core/memory/memory_objects.c src/core/memory/simple_manifest.c
libpull_a_SOURCES += src/core/memory/memory_impl.c src/core/memory/memory_flash.c
libpull_a_SOURCES += src/core/network/receiver.c src/core/network/subscriber.c src/core/network/connection_config.c src/core/network/writer.c
libpull_a_SOURCES += src/core/security/verifier.c src/core/security/tinycrypt.c src/core/security/tinydtls.c src/core/security/cryptoauthlib.c
libpull_a_SOURCES += src/core/pipeline/lzss.c src/core/pipeline/bspatch.c src/core/pipeline/buffer.c

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
