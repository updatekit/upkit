CC=gcc
INCLUDE=-I../../ext/libcoap/include -I../../ext/tinydtls
INCLUDE+=-Iresources -I../../include
RESOURCES=$(shell find resources -name "*.c" -print)
LIBS=../../ext/libcoap/.libs/libcoap-1.a
LIBS+=../../ext/tinydtls/libtinydtls.a

build:
	$(CC) -Wall $(INCLUDE) server.c network.c ../../src/memory/manifest.c -DSIMPLE_MANIFEST \
			../../src/security/tinydtls.c  -DWITH_TINYDTLS\
			../../src/memory/simple_manifest_impl.c $(RESOURCES) $(LIBS) -o server.bin
