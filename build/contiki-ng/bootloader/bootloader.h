#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#define FIRST_BOOT 0x1

typedef struct {
    uint8_t vendor_key[64];
    uint8_t buffer[63];
    uint8_t startup_flags;
} bootloader_ctx;

#endif
