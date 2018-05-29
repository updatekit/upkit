#include "ti-lib.h"
#include "contiki.h"
#include "driverlib/flash.h"
#include "board-peripherals.h"

#include <libpull/common.h>
#include "memory_cc2650.h"

#include <string.h>

#define FLASH_PAGE_SIZE PAGE_SIZE // Defined in Manifest.conf

const mem_slot_t memory_slots[] = { 
    {
        .id = OBJ_RUN,
        .bootable = true,
        .loaded = true
    },
    {
        .id = OBJ_1,
        .bootable = false,
        .loaded = false
    },
    {
        .id = OBJ_2,
        .bootable = false,
        .loaded = false
    },
    {OBJ_END}
};

static uint8_t external_memory_open = 0;

int memory_object_mapper[] = {
     [BOOTLOADER_CTX] = INTERNAL,
     [OBJ_GOLD] = EXTERNAL,
     [OBJ_RUN] = INTERNAL,
     [OBJ_1] = EXTERNAL,
     [OBJ_2] = EXTERNAL,
};

int memory_object_start[] = {
     [BOOTLOADER_CTX] = BOOTLOADER_CTX_START_OFFSET, // Defined in Makefile.conf
     [OBJ_GOLD] = 0x1A000,
     [OBJ_RUN] = IMAGE_START_PAGE*FLASH_PAGE_SIZE, // Defined in Makefile.conf
     [OBJ_1] = 0x34000,
     [OBJ_2] = 0x4E000
};

int memory_object_end[] = {
     [BOOTLOADER_CTX] = BOOTLOADER_CTX_END_OFFSET, // Defined in Makefile.conf
     [OBJ_GOLD] = 0x34000,
     [OBJ_RUN] = IMAGE_END_PAGE*FLASH_PAGE_SIZE, // Defined in Makefile.conf
     [OBJ_1] = 0x4E000,
     [OBJ_2] = 0x68000
};

pull_error memory_open_impl(mem_object_t* ctx, mem_id_t obj, mem_mode_t mode) {
    ctx->type = memory_object_mapper[obj]; // XXX this can fail if the obj is invalid
    ctx->start_offset = memory_object_start[obj];
    ctx->end_offset = memory_object_end[obj];
    PULL_ASSERT(mode > READ_ONLY && mode < WRITE_ALL);
    PULL_ASSERT(ctx->type == EXTERNAL || ctx->type == INTERNAL);
    ctx->mode = mode;
    address_t offset;
    if (ctx->type == EXTERNAL) {
        if (external_memory_open == 0) {
            if (ext_flash_open(NULL)) {
                external_memory_open = 1;
            } else {
                log_debug("Cannot open external flash\n");
                ext_flash_close(NULL);
                return MEMORY_OPEN_ERROR;
            }
        } else {
            external_memory_open++;
        }
        if (ctx->mode == WRITE_ALL) {
            for (offset = ctx->start_offset; offset < ctx->end_offset; offset += FLASH_PAGE_SIZE) {
                if (!ext_flash_erase(NULL, offset, FLASH_PAGE_SIZE)) {
                    log_debug("Error ereasing page %lu\n", offset/FLASH_PAGE_SIZE);
                    return MEMORY_OPEN_ERROR;
                }
            }
            log_debug("pages erased correctly\n");
        }
        return PULL_SUCCESS;
    }
    // Internal Memory
    if (ctx->mode == WRITE_ALL) {
        for (offset = ctx->end_offset; offset < ctx->end_offset; offset += FLASH_PAGE_SIZE) {
            if (FlashSectorErase(offset) != FAPI_STATUS_SUCCESS) {
                log_debug("Error erasing page %lu\n", offset/FLASH_PAGE_SIZE);
                return MEMORY_OPEN_ERROR;
            }
        }
    }
    return PULL_SUCCESS;
}

int memory_read_impl(mem_object_t* ctx, void* memory_buffer, address_t size, address_t offset) {
    PULL_ASSERT(ctx->mode == READ_ONLY);
    // TODO implement a check for the end offset
    if (ctx->type == EXTERNAL) {
        if (!ext_flash_read(NULL, ctx->start_offset+offset, size, memory_buffer)) {
            log_debug("Error reading external memory\n");
            return -1;
        }
        return size;
    } else if (ctx->type == INTERNAL) {
        uint8_t* memory_offset = (uint8_t*) (ctx->start_offset+offset);
        if (memcpy(memory_buffer, memory_offset, size) != memory_buffer) {
            log_debug("Error reading internal memory\n");
            return -1;
        }
        return size;
    }
    return -1; 
}

pull_error memory_flush_impl(mem_object_t* ctx) {
    return PULL_SUCCESS;
}

int memory_write_impl(mem_object_t* ctx, const void* memory_buffer, uint16_t size, address_t offset) {
    PULL_ASSERT(ctx->type == EXTERNAL || ctx->type == INTERNAL);
    if (ctx->type == EXTERNAL) {
        if (!ext_flash_write(NULL, ctx->start_offset+offset, size, memory_buffer)) {
            log_debug("Error writing into external flash\n");
            return -1;
        }
        return size;
    }
    // Internal Memory
    address_t memory_offset = ctx->start_offset+offset;
    if (FlashProgram((uint8_t*) memory_buffer, memory_offset, size) != FAPI_STATUS_SUCCESS) {
        return -1
    }
    return size;
}

pull_error memory_close_impl(mem_object_t* ctx) {
    PULL_ASSERT(ctx->type == EXTERNAL || ctx->type == INTERNAL);
    if (ctx->type == EXTERNAL) {
        //if (--external_memory_open == 0) {
        //ext_flash_close(NULL); //XXX I need to check how safe is to close the memory
        //}
        return PULL_SUCCESS;
    }
    return PULL_SUCCESS;
}
