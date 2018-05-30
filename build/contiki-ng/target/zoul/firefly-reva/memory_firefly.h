#ifndef MEMORY_FIREFLY_H_
#define MEMORY_FIREFLY_H_

#include <libpull/memory.h>

struct mem_object_t {
    uint32_t start_offset;
    uint32_t end_offset;
    mem_mode_t mode;
};

enum memory_objects_enum {
    BOOTLOADER,
    BOOTLOADER_CTX,
    OBJ_1,
    OBJ_2,
};

pull_error memory_open_impl(mem_object_t* ctx, mem_id_t id, mem_mode_t mode);

int memory_read_impl(mem_object_t* ctx, void* memory_buffer, size_t size, address_t offset);

int memory_write_impl(mem_object_t* ctx, const void* memory_buffer, size_t size, address_t offset);

pull_error memory_flush_impl(mem_object_t* ctx);

pull_error memory_close_impl(mem_object_t* ctx);

#endif /* MEMORY_FIREFLY_H_ */
