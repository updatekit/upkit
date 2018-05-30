#ifndef MEMORY_CC2650_H_
#define MEMORY_CC2650_H_

#include <libpull/memory.h>

enum memory_type {
    EXTERNAL,
    INTERNAL,
    INVALID
};

typedef struct mem_object_t {
    enum memory_type type;
    address_t start_offset;
    address_t end_offset;
    mem_mode_t mode;
} mem_object_t;

enum memory_objects_enum {
    OBJ_FIRST = 0,
    BOOTLOADER_CTX, // 1
    OBJ_GOLD,       // 2
    OBJ_RUN,        // 3
    OBJ_1,          // 4
    OBJ_2,          // 5
    OBJ_LAST        // 6
};

pull_error memory_open_impl(mem_object_t* ctx, mem_id_t id, mem_mode_t mode);

int memory_read_impl(mem_object_t* ctx, void* memory_buffer, address_t size, address_t offset);

int memory_write_impl(mem_object_t* ctx, const void* memory_buffer, uint16_t size, uint32_t offset);

pull_error memory_flush_impl(mem_object_t* ctx);

pull_error memory_close_impl(mem_object_t* ctx);

#endif // MEMORY_CC2650_H_
