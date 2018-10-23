#ifdef WITH_PLATFORM_CC2650

#include "ti-lib.h"
#include "contiki.h"
#include "driverlib/flash.h"
#include "board-peripherals.h"

#include <libpull/common.h>
#include <libpull/memory.h>

#include <string.h>

#define FLASH_PAGE_SIZE PAGE_SIZE // Defined in Manifest.conf

const mem_slot_t memory_slots[] = { 
    {
        .id = OBJ_1,
        .bootable = true,
        .reserved = false
    },
    {
        .id = OBJ_2,
        .bootable = false,
        .reserved = false
    },
    {OBJ_END}
};

static int internal_flash_erase(address_t offset, size_t page_size) {
    if (FlashSectorErase(offset) != FAPI_STATUS_SUCCESS) {
        log_debug("Error erasing page %lu\n", offset/page_size);
        return -1;
    }
    return 0;
}
static int internal_flash_program(const uint8_t* buffer, address_t offset, size_t size) {
    if (FlashProgram((uint8_t*) buffer, offset, size) != FAPI_STATUS_SUCCESS) {
        return -1;
    }
    return 0;

}
static int internal_flash_read(uint8_t* buffer, address_t offset, size_t size) {
    if (memcpy(buffer, (char*) offset, size) != buffer) {
        return -1;
    }
    return 0;
}
static flash_descr_t internal_flash_descr = {
    .page_size = FLASH_PAGE_SIZE,
    .open = NULL, /* not necessary */
    .erase = internal_flash_erase,
    .program = internal_flash_program,
    .read = internal_flash_read,
    .close = NULL, /* not necessary */
    .rst = NULL, /* not necessary */
};


static uint8_t external_memory_open = 0;
static int external_flash_open(void) {
    if (external_memory_open == 0) {
        if (ext_flash_open(NULL)) {
            external_memory_open = 1;
            return 0;
        } else {
            log_debug("Cannot open external flash\n");
            ext_flash_close(NULL);
            return -1;
        }
    }
    external_memory_open++;
    return 0;
}

static int external_flash_erase(address_t offset, size_t page_size) {
    if (!ext_flash_erase(NULL, offset, page_size)) {
        log_debug("Error ereasing page %lu\n", offset/page_size);
        return -1;
    }
    return 0;
}
static int external_flash_program(const uint8_t* buffer, address_t offset, size_t size) {
    if (!ext_flash_write(NULL, offset, size, buffer)) {
        log_debug("Error writing into external flash\n");
        return -1;
    }
    return 0;
}
static int external_flash_read(uint8_t* buffer, address_t offset, size_t size) {
    if (!ext_flash_read(NULL, offset, size, buffer)) {
        return -1;
    }
    return 0;
}
int external_flash_close(void) {
    if (external_memory_open == 1) {
        ext_flash_close(NULL);
    }
    external_memory_open--;
    return 0;
}
static flash_descr_t external_flash_descr = {
    .page_size = FLASH_PAGE_SIZE,
    .open = external_flash_open,
    .erase = external_flash_erase,
    .program = external_flash_program,
    .read = external_flash_read,
    .close = external_flash_open,
    .rst = NULL, /* not necessary */
};

// PAGE_SIZE is defined in Makefile.conf
#define BOOTLOADER_SIZE (BOOTLOADER_END_PAGE-BOOTLOADER_START_PAGE)*PAGE_SIZE
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
        .start = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE,
        .end = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + IMAGE_SIZE,
        .fdescr = &internal_flash_descr
    },
    [OBJ_2] = {
        .start = 0x34000,
        .end = 0x4E000,
        .fdescr = &external_flash_descr
    },
};
#endif /* WITH_PLATFORM_CC2650 */
