AUTOMAKE_OPTIONS = subdir-objects

CFLAGS = -Wall -Iinclude -Iext/tinycrypt/lib/include -Iext/tinydtls

noinst_LIBRARIES = libpull.a

libpull_a_SOURCES = src/common/error.c 
libpull_a_SOURCES += src/memory/manifest.c src/memory/memory_objects.c src/memory/simple_manifest_impl.c
libpull_a_SOURCES += src/network/receiver.c src/network/subscriber.c
libpull_a_SOURCES += src/security/verifier.c src/security/tinycrypt.c src/security/tinydtls.c src/security/cryptoauthlib.c

pkginclude_HEADERS = include/common/error.h
