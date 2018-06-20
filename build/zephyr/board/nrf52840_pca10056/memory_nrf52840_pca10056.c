#include "memory_nrf52840_pca10056.h"

#include <flash.h>

#include <libpull/memory.h>
#include <libpull/common.h>
#include <string.h>

const mem_slot_t memory_slots[] = {
    {
        .id = OBJ_1,
        .bootable = true,
        .loaded = true
    },
    {
        .id = OBJ_2,
        .bootable = false,
        .loaded = false
    },
    {OBJ_END}
};

// Since we are working only with one flash driver we can define it as a static
// variable

static struct device *flash_device;

static int internal_flash_open(void) {
    if (flash_device == NULL) {
        flash_device = device_get_binding(FLASH_DEV_NAME);
    }
    return flash_device == NULL? 1: 0;
}

// O on success
static int internal_flash_erase(address_t offset, size_t page_size) {
    return flash_erase(flash_device, offset, page_size);
}
// 0 on success
static int internal_flash_program(const uint8_t* buffer, address_t offset, size_t size) {
    flash_write_protection_set(flash_device, false);
    uint8_t value = flash_write(flash_device, offset, buffer, size);
    flash_write_protection_set(flash_device, true);
    return value;
}
// 0 on success
static int internal_flash_read(uint8_t* buffer, address_t offset, size_t size) {
    return flash_read(flash_device, offset, buffer, size);
}

#define PAGE_SIZE 0x1000
static flash_descr_t internal_flash_descr = {
    .page_size = PAGE_SIZE,
    .open = internal_flash_open,
    .erase = internal_flash_erase,
    .program = internal_flash_program,
    .read = internal_flash_read,
    .close = NULL, /* not necessary */
    .rst = NULL
};
/*
The flash is divided into 256 pages of 4 kB each that can be accessed by the CPU
via both the ICODE and DCODE.
*/

// PAGE_SIZE is defined in Makefile.conf
//#define BOOTLOADER_SIZE (BOOTLOADER_END_PAGE-BOOTLOADER_START_PAGE)*PAGE_SIZE
#define IMAGE_START_PAGE 0
#define IMAGE_END_PAGE 100

#define IMAGE_SIZE (IMAGE_END_PAGE-IMAGE_START_PAGE)*PAGE_SIZE

#define INITIAL_MEMORY_OFFSET 0
#define BOOTLOADER_SIZE 0
#define BOOTLOADER_CTX_START_OFFSET 0
#define BOOTLOADER_CTX_END_OFFSET 0

mem_object_t flash_objects[] = {
    [BOOTLOADER] = {
        .start = INITIAL_MEMORY_OFFSET,
        .end = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE,
        .fdescr = &internal_flash_descr
    },
    [BOOTLOADER_CTX] = {
        .start = INITIAL_MEMORY_OFFSET + BOOTLOADER_CTX_START_OFFSET,
        .end = INITIAL_MEMORY_OFFSET + BOOTLOADER_CTX_END_OFFSET,
        .fdescr = &internal_flash_descr
    },
    [OBJ_1] = {
        .start = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE,
        .end = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + IMAGE_SIZE,
        .fdescr = &internal_flash_descr
    },
    [OBJ_2] = {
        .start = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + IMAGE_SIZE,
        .end = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + 2*IMAGE_SIZE,
        .fdescr = &internal_flash_descr
    },
    [SWAP] = {
        .start = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + 2*IMAGE_SIZE,
        .end = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + 2*IMAGE_SIZE + PAGE_SIZE,
        .fdescr = &internal_flash_descr
    }
};
