#ifndef MEMORY_CC2650_H_
#define MEMORY_CC2650_H_

#include "memory/memory.h"
#include "memory/memory_objects.h"

enum memory_type {
    EXTERNAL,
    INTERNAL,
    INVALID
};

struct mem_object_ {
    enum memory_type type;
    address_t start_offset;
    address_t end_offset;
    mem_mode_t mode;
};

enum memory_objects_enum {
    OBJ_FIRST = 0,
    BOOTLOADER_CTX,
    OBJ_GOLD,
    OBJ_RUN,
    OBJ_1,
    OBJ_2,
    OBJ_LAST
};

pull_error memory_open_impl(mem_object* ctx, obj_id id, mem_mode_t mode);

int memory_read_impl(mem_object* ctx, void* memory_buffer, uint16_t size, uint32_t offset);

int memory_write_impl(mem_object* ctx, const void* memory_buffer, uint16_t size, uint32_t offset);

pull_error memory_flush_impl(mem_object* ctx);

pull_error memory_close_impl(mem_object* ctx);

#endif // MEMORY_CC2650_H_
