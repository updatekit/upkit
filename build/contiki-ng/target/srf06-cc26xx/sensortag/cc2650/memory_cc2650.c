#include <string.h>

#include "common/external.h"
#include "memory_cc2650.h"
#include "common/logger.h"
#include "common/error.h"

#include "ti-lib.h"
#include "contiki.h"
#include "driverlib/flash.h"
#include "board-peripherals.h"
#include "memory_cc2650.h"

#define BUFFER_SIZE 0x100

static uint32_t buffer_offset = 0; // it is signed
static uint16_t buffer_full = 0;
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
     [OBJ_RUN] = 0x5000,
     [OBJ_GOLD] = 0x1A000,
     [OBJ_1] = 0x34000,
     [OBJ_2] = 0x4E000,
     [BOOTLOADER_CTX] = 0x1F000
};

pull_error memory_open_impl(mem_object* ctx, obj_id obj) {
    ctx->type = memory_object_mapper[obj];
    ctx->start_offset = memory_object_start[obj];
    if (ctx->type == EXTERNAL) {
        if (external_memory_open == 1) {
            return PULL_SUCCESS;
        }
        if(!ext_flash_open()) {
            log_error(MEMORY_OPEN_ERROR, "Failure opening external flash\n");
            ext_flash_close();
            return -1;
        }
        external_memory_open = 1;
        return PULL_SUCCESS;
    } else if (ctx->type == INTERNAL) {
        return PULL_SUCCESS;
    }
    return MEMORY_OPEN_ERROR;
}

int memory_read_impl(mem_object* ctx, void* memory_buffer, uint16_t size, uint32_t offset) {
    if (ctx->type == EXTERNAL) {
        if (!ext_flash_read(ctx->start_offset+offset, size, memory_buffer)) {
            log_error(MEMORY_READ_ERROR, "Error reading external memory\n");
            return -1;
        }
        return size;
    } else if (ctx->type == INTERNAL) {
        // Reading the cc2650 internal memory is as easy as accessing direcly
        // the memory offset
        uint8_t* memory_offset = (uint8_t*) (ctx->start_offset+offset);
        if (memcpy(memory_buffer, memory_offset, size) != memory_buffer) {
            log_error(MEMORY_READ_ERROR, "Error reading internal memory\n");
            return -1;
        }
        return size;
    }
    return -1; 
}

pull_error memory_flush_impl(mem_object* ctx) {
    if (buffer_full == 0) {
        return PULL_SUCCESS;
    }
    if (!ext_flash_erase(buffer_offset, buffer_full)) {
        log_error(MEMORY_ERASE_ERROR, "Error erasing erasing flash\n");
        //ext_flash_close();
        return MEMORY_FLUSH_ERROR;
    }
    if (!ext_flash_write(buffer_offset, buffer_full, buffer)) {
        log_error(MEMORY_WRITE_ERROR, "Error writing into external flash\n");
        //ext_flash_close();
        return MEMORY_FLUSH_ERROR;
    }
    log_debug("Flushing buffer of size %d\n", buffer_full);
    buffer_full = 0;
    buffer_offset = 0;
    return PULL_SUCCESS;
}

int memory_write_impl(mem_object* ctx, const void* memory_buffer, uint16_t size, uint32_t offset) {
    if (ctx->type == EXTERNAL) {
        if ((uint32_t) ctx->start_offset+offset < buffer_offset+BUFFER_SIZE && 
                (uint32_t) ctx->start_offset+offset > buffer_offset) {
            if (buffer_full+size <= BUFFER_SIZE) {
                memcpy(&buffer[buffer_full], memory_buffer, size); /// XXX Check for errors
                buffer_full+=size; // Questo è un errore, perchè magari stavo sovrascrivendo qualcosa
            } else {
                uint16_t chunk_size = BUFFER_SIZE-buffer_full;
                memcpy(&buffer[buffer_full], memory_buffer, chunk_size);
                if (memory_flush_impl(ctx) != PULL_SUCCESS) {
                    log_error(MEMORY_WRITE_ERROR, "Error while writing the buffer\n");
                    return -1;
                }
                buffer_offset = ctx->start_offset+offset+chunk_size;
                memcpy(&buffer[buffer_full], memory_buffer+chunk_size, size-chunk_size);
            }
            return size;
        }
        // We are not in the page pointed by the buffer, flush the buffer and
        // update the buffer_offset
        pull_error err = memory_flush_impl(ctx);
        if (err) {
            log_error(err, "Error while writing the buffer\n");
            return -1;
        }
        buffer_offset = ctx->start_offset+offset;
        memcpy(&buffer[buffer_full], memory_buffer, size); /// XXX Check for errors
        buffer_full+=size;
        return size;
    } else if (ctx->type == INTERNAL) {
        uint32_t memory_offset = ctx->start_offset+offset;
        int try = 3;
        while (try--) {
            if (FlashProgram((uint8_t*) memory_buffer, memory_offset, size) == FAPI_STATUS_SUCCESS) {
                log_debug("Correcly written into internal memory %u bytes\n", size);
                break;
            }
            log_debug("Try again to write into internal memory\n");
        }
        if (try == 0) {
            log_error(MEMORY_WRITE_ERROR, "Error writing into internal memory\n");
            return -1;
        }
        return size;
    }
    return -1;
}

pull_error memory_close_impl(mem_object* ctx) {
    if (ctx->type == EXTERNAL) {
        if (memory_flush_impl(ctx) != PULL_SUCCESS) {
            return MEMORY_CLOSE_ERROR;
        }
        //ext_flash_close(); XXX I need to check how safe is to close the memory
        return PULL_SUCCESS;
    } else if (ctx->type == INTERNAL) {
        return PULL_SUCCESS;
    }
    return MEMORY_CLOSE_ERROR;
}
