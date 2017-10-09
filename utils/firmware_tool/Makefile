firmware_tools: firmware_tools.c
	gcc -Wall -o firmware_tools -I../../include -I../../ext/tinycrypt/lib/include \
			firmware_tools.c ../../ext/tinycrypt/lib/source/ecc_platform_specific.c \
			../../src/memory/metadata.c \
			../../src/memory/simple_metadata.c \
			../../ext/tinycrypt/lib/libtinycrypt.a

clean:
	rm firmware_tools
