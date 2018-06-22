AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = test

AM_CFLAGS = -Wall -I. -Iinclude -Iext/tinycrypt/lib/include -Iext/tinydtls -DHAVE_CONFIG_H -ffreestanding

noinst_LIBRARIES = libpull.a

libpull_a_SOURCES = src/common/error.c src/common/identity.c
libpull_a_SOURCES += src/memory/manifest.c src/memory/memory_objects.c src/memory/simple_manifest.c
libpull_a_SOURCES += src/memory/memory_impl.c src/memory/memory_flash.c
libpull_a_SOURCES += src/network/receiver.c src/network/subscriber.c src/network/connection_config.c src/network/writer.c
libpull_a_SOURCES += src/security/verifier.c src/security/tinycrypt.c src/security/tinydtls.c src/security/cryptoauthlib.c

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
