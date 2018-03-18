#include <string.h>

#include "common/libpull.h"
#include "memory_cc2650.h"

#include "ti-lib.h"
#include "contiki.h"
#include "driverlib/flash.h"
#include "board-peripherals.h"
#include "memory_cc2650.h"

#define FLASH_PAGE_SIZE PAGE_SIZE // Defined in Manifest.conf
#define BUFFER_SIZE 0x100

static address_t buffer_offset = 0; // it is signed
static address_t buffer_full = 0;
static uint8_t buffer[BUFFER_SIZE];

const int8_t memory_objects[] = { OBJ_1, OBJ_2, OBJ_END};

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
     [BOOTLOADER_CTX] = BOOTLOADER_CTX_END_OFFSET,
     [OBJ_GOLD] = 0x34000,
     [OBJ_RUN] = IMAGE_END_PAGE*FLASH_PAGE_SIZE,
     [OBJ_1] = 0x4E000,
     [OBJ_2] = 0x68000
};

pull_error memory_open_impl(mem_object* ctx, obj_id obj, mem_mode_t mode) {
    ctx->type = memory_object_mapper[obj]; // XXX this can fail if the obj is invalid
    ctx->start_offset = memory_object_start[obj];
    ctx->end_offset = memory_object_end[obj];
    PULL_ASSERT(mode > READ_ONLY && mode < WRITE_ALL);
    PULL_ASSERT(ctx->type == EXTERNAL || ctx->type == INTERNAL);
    ctx->mode = mode;
    address_t offset;
    if (ctx->type == EXTERNAL && external_memory_open == 0 && ext_flash_open()) {
        external_memory_open = 1;
    } else {
        log_debug("Cannot open external flash\n");
        return MEMORY_OPEN_ERROR;
    }
    external_memory_open++;
    if (ctx->type == EXTERNAL && ctx->mode == WRITE_ALL) {
        for (offset = ctx->end_offset; offset < ctx->end_offset; offset += FLASH_PAGE_SIZE) {
            if (!ext_flash_erase(offset, FLASH_PAGE_SIZE)) {
                log_debug("Error ereasing page %d\n", offset/FLASH_PAGE_SIZE);
                return MEMORY_OPEN_ERROR;
            }
        }
    } else if (ctx->type == INTERNAL && ctx->mode == WRITE_ALL) {
        for (offset = ctx->end_offset; offset < ctx->end_offset; offset += FLASH_PAGE_SIZE) {
            if (FlashSectorErase(offset) != FAPI_STATUS_SUCCESS) {
                log_debug("Error erasing page %d\n", offset/FLASH_PAGE_SIZE);
                return MEMORY_OPEN_ERROR;
            }
        }
    }
    return PULL_SUCCESS;
}

int memory_read_impl(mem_object* ctx, void* memory_buffer, uint16_t size, uint32_t offset) {
    PULL_ASSERT(ctx->mode == READ_ONLY);
    // TODO implement a check for the end offset
    if (ctx->type == EXTERNAL) {
        if (!ext_flash_read(ctx->start_offset+offset, size, memory_buffer)) {
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

pull_error memory_flush_impl(mem_object* ctx) {
    PULL_ASSERT(ctx->type == EXTERNAL || ctx->type == INTERNAL);
    if (ctx->type == INTERNAL) {
        // The internal memory is not buffered
        return PULL_SUCCESS;
    }
    if (!ext_flash_write(buffer_offset, buffer_full, buffer)) {
        log_debug("Error writing into external flash\n");
        return MEMORY_FLUSH_ERROR;
    }
    log_debug("\nFlushing buffer of size %d\n", buffer_full);
    buffer_full = 0;
    buffer_offset = 0;
    return PULL_SUCCESS;
}

int memory_write_impl(mem_object* ctx, const void* memory_buffer, uint16_t size, address_t offset) {
    PULL_ASSERT(ctx->type == EXTERNAL || ctx->type == INTERNAL);
    // TODO implement a check for the end offset
    if (ctx->type == EXTERNAL) {
        if ((address_t) ctx->start_offset+offset < buffer_offset+BUFFER_SIZE && 
                (address_t) ctx->start_offset+offset > buffer_offset) {
            if (buffer_full+size <= BUFFER_SIZE) {
                memcpy(&buffer[buffer_full], memory_buffer, size); /// XXX Check for errors
                buffer_full+=size; // Questo è un errore, perchè magari stavo sovrascrivendo qualcosa
            } else {
                uint16_t chunk_size = BUFFER_SIZE-buffer_full;
                memcpy(&buffer[buffer_full], memory_buffer, chunk_size); /// XXX Check for errors
                if (memory_flush_impl(ctx) != PULL_SUCCESS) {
                    log_debug("Error while writing the buffer\n");
                    return -1;
                }
                buffer_offset = ctx->start_offset+offset+chunk_size;
                memcpy(&buffer[buffer_full], memory_buffer+chunk_size, size-chunk_size); /// XXX Check for errors
            }
            return size;
        }
        // We are not in the page pointed by the buffer, flush the buffer and
        // update the buffer_offset
        pull_error err = memory_flush_impl(ctx);
        if (err) {
            log_debug("Error writing the buffer\n");
            return -1;
        }
        buffer_offset = ctx->start_offset+offset;
        memcpy(&buffer[buffer_full], memory_buffer, size); /// XXX Check for errors
        buffer_full+=size;
        return size;
    }
    // Internal Memory
    address_t memory_offset = ctx->start_offset+offset;
    uint8_t try = 3;
    while (try--) {
        if (FlashProgram((uint8_t*) memory_buffer, memory_offset, size) == FAPI_STATUS_SUCCESS) {
            log_debug("Correcly written into internal memory %u bytes\n", size);
            break;
        }
        log_debug("Try again to write into internal memory\n");
    }
    if (try == 0) {
        log_debug("Error writing into internal memory\n");
        return -1;
    }
    return size;
}

pull_error memory_close_impl(mem_object* ctx) {
    PULL_ASSERT(ctx->type == EXTERNAL || ctx->type == INTERNAL);
    if (ctx->type == EXTERNAL) {
        if (memory_flush(ctx) != PULL_SUCCESS) {
            return MEMORY_CLOSE_ERROR;
        }
        if (--external_memory_open == 0) {
            ext_flash_close(); //XXX I need to check how safe is to close the memory
        }
        return PULL_SUCCESS;
    }
    return PULL_SUCCESS;
}
