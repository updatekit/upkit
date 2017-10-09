CC=gcc
INCLUDE=-I../../ext/libcoap/include
INCLUDE+=-Iresources -I../../include
RESOURCES=$(shell find resources -name "*.c" -print)
LIBS=../../ext/libcoap/.libs/libcoap-1.a
LIBS+=../../ext/tinydtls/libtinydtls.a

build:
	$(CC) -Wall $(INCLUDE) server.c network.c ../../src/memory/metadata.c \
			../../src/memory/simple_metadata.c $(RESOURCES) $(LIBS) -o server.bin
