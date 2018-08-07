# Build the bootloade context

all: bootloader_ctx
	./bootloader_ctx bootloader_ctx.bin

bootloader_ctx: bootloader_ctx.c
	$(CC) -o $@ -I../../../include $<

