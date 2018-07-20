#ifdef WITH_PLATFORM_NRF52840

#include "platform_headers.h"
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

#define PAGE_SIZE 0x1000
#define INITIAL_MEMORY_OFFSET 0x0
#define BOOTLOADER_START_PAGE 0
#define BOOTLOADER_END_PAGE 8
#define IMAGE_START_PAGE 9
#define IMAGE_END_PAGE 109
#define MANIFEST_SIZE 0x100
#define BOOTLOADER_CTX_START_OFFSET 0x8000
#define BOOTLOADER_CTX_END_OFFSET 0x9000

#define BOOTLOADER_SIZE (BOOTLOADER_END_PAGE-BOOTLOADER_START_PAGE)*PAGE_SIZE
#define BOOTLOADER_CTX_SIZE (BOOTLOADER_CTX_END_OFFSET-BOOTLOADER_CTX_START_OFFSET)
#define IMAGE_OFFSET BOOTLOADER_SIZE+BOOTLOADER_CTX_SIZE
#define IMAGE_SIZE (IMAGE_END_PAGE-IMAGE_START_PAGE)*PAGE_SIZE

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
        .start = INITIAL_MEMORY_OFFSET + IMAGE_OFFSET,
        .end = INITIAL_MEMORY_OFFSET + IMAGE_OFFSET + IMAGE_SIZE,
        .fdescr = &internal_flash_descr
    },
    [OBJ_2] = {
        .start = INITIAL_MEMORY_OFFSET + IMAGE_OFFSET + IMAGE_SIZE,
        .end = INITIAL_MEMORY_OFFSET + IMAGE_OFFSET + 2*IMAGE_SIZE,
        .fdescr = &internal_flash_descr
    },
    [SWAP] = {
        .start = INITIAL_MEMORY_OFFSET + IMAGE_OFFSET + 2*IMAGE_SIZE,
        .end = INITIAL_MEMORY_OFFSET + IMAGE_OFFSET + 2*IMAGE_SIZE + PAGE_SIZE,
        .fdescr = &internal_flash_descr
    }
};

#endif /* WITH_PLATFORM_NRF52840 */
