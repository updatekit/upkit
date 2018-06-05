#include "contiki.h"
#include "cpu.h"
#include "dev/cc2538-dev.h"
#include "memory_firefly.h"
#include "dev/rom-util.h" // cc2538 specific file
#include "dev/flash.h" // cc2538 specific file
#include "./utils.h"
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

// PAGE_SIZE is defined in Makefile.conf
#define BOOTLOADER_SIZE (BOOTLOADER_END_PAGE-BOOTLOADER_START_PAGE)*PAGE_SIZE
#define IMAGE_SIZE (IMAGE_END_PAGE-IMAGE_START_PAGE)*PAGE_SIZE

uint32_t memory_object_start[] = {
    [BOOTLOADER] = INITIAL_MEMORY_OFFSET,
    [BOOTLOADER_CTX] = INITIAL_MEMORY_OFFSET + BOOTLOADER_CTX_START_OFFSET,
    [OBJ_1] = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE,
    [OBJ_2] = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + IMAGE_SIZE,
    [SWAP] = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + 2*IMAGE_SIZE,
};
uint32_t memory_object_end[] = {
    [BOOTLOADER] = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE,
    [BOOTLOADER_CTX] = INITIAL_MEMORY_OFFSET + BOOTLOADER_CTX_END_OFFSET,
    [OBJ_1] = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + IMAGE_SIZE,
    [OBJ_2] = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + 2*IMAGE_SIZE,
    [SWAP] = INITIAL_MEMORY_OFFSET + BOOTLOADER_SIZE + 2*IMAGE_SIZE + PAGE_SIZE
};

uint32_t get_start_offset(mem_id_t id) {
    return memory_object_start[id];
}
uint32_t get_end_offset(mem_id_t id) {
    return memory_object_end[id];
}

pull_error memory_open_impl(mem_object_t* ctx, mem_id_t obj, mem_mode_t mode) {
    address_t offset;
    ctx->mode = mode;
    ctx->start_offset = memory_object_start[obj];
    ctx->end_offset = memory_object_end[obj];
    if (mode == WRITE_ALL) {
        for (offset = ctx->start_offset; offset < ctx->end_offset; offset += PAGE_SIZE) {
            // This is needed since it is not possible to stop the watchdog in
            // the cc2538
            watchdog_periodic();
            int ret = rom_util_page_erase(offset, PAGE_SIZE);
            if (ret != 0) {
                log_error(MEMORY_ERASE_ERROR, "Error erasing page\n");
                return MEMORY_OPEN_ERROR;
            }
        }
    }
    return PULL_SUCCESS;
}

int memory_read_impl(mem_object_t* ctx, void* memory_buffer, size_t size, address_t offset) {
    uint32_t* address = (uint32_t*) ((uint32_t)(ctx->start_offset+offset));
    if (memcpy(memory_buffer, address, size) != memory_buffer) {
        log_error(MEMORY_READ_ERROR, "Error reading internal memory\n");
        return -1;
    }
    return size;
}
pull_error memory_flush_impl(mem_object_t* ctx) {
    // No flushing implemented
    return PULL_SUCCESS;
}

int memory_write_impl(mem_object_t* ctx, const void* memory_buffer, size_t size, address_t offset) {
    // Using cc2538 ROM functions to interact with the flash
    // http://www.ti.com/lit/ug/swru333a/swru333a.pdf
    INTERRUPTS_DISABLE();
    // Erase the page if we are in sequential write
    if (ctx->mode == SEQUENTIAL_REWRITE && ((ctx->start_offset + offset) % PAGE_SIZE) == 0) {
        printf("Erasing page at offset %x\n", (ctx->start_offset + offset));
        int ret = rom_util_page_erase(ctx->start_offset + offset, PAGE_SIZE);
        if (ret != 0) {
            log_error(MEMORY_ERASE_ERROR, "Error erasing page\n");
            return MEMORY_WRITE_ERROR;
        }
    }
    int32_t ret = rom_util_program_flash((uint32_t*) memory_buffer, ctx->start_offset+offset, size);
    INTERRUPTS_ENABLE();
    if (ret != 0) {
        log_error(MEMORY_WRITE_ERROR, "Error writing into flash\n");
        return -1;
    }
    return size;
}

pull_error memory_close_impl(mem_object_t* ctx) {
    // We have only an internal memory and we do not need to close it
    return PULL_SUCCESS;
}
