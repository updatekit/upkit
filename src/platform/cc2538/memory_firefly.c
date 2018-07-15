#include "memory_firefly.h"
#include "./rom.h"

#include "./utils.h"

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

// O on success
static int internal_flash_erase(address_t offset, size_t page_size) {
    INTERRUPTS_DISABLE();
    int ret = ROM_PageErase(offset, page_size);
    INTERRUPTS_ENABLE();
    return ret;
}
// 0 on success
static int internal_flash_program(const uint8_t* buffer, address_t offset, size_t size) {
    INTERRUPTS_DISABLE();
    int ret = ROM_ProgramFlash((uint32_t*) buffer, offset, size);
    INTERRUPTS_ENABLE();
    return ret;
}
// 0 on success
static int internal_flash_read(uint8_t* buffer, address_t offset, size_t size) {
    if (ROM_Memcpy(buffer, (void*) offset, size) != buffer) {
        return -1;
    }
    return 0;
}

static flash_descr_t internal_flash_descr = {
    .page_size = PAGE_SIZE,
    .open = NULL, /* not necessary */
    .erase = internal_flash_erase,
    .program = internal_flash_program,
    .read = internal_flash_read,
    .close = NULL, /* not necessary */
    .rst = NULL
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
