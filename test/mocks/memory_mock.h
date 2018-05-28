#ifndef MEMORY_MOCK_H_
#define MEMORY_MOCK_H_

#include <libpull/memory/memory_interface.h>

typedef struct {
    pull_error (*memory_open_impl) (mem_object_t*, mem_id_t, mem_mode_t);
    uint16_t (*memory_read_impl)(mem_object_t*, void*, uint16_t, uint32_t);
    uint16_t (*memory_write_impl)(mem_object_t*, const void*, uint16_t, uint32_t);
    pull_error (*memory_flush_impl)(mem_object_t*);
    pull_error (*memory_close_impl)(mem_object_t*);
} memory_mock_t;

extern memory_mock_t memory_mock;

void memory_mock_restore();

pull_error memory_open_invalid(mem_object_t* ctx, mem_id_t obj, mem_mode_t);
uint16_t memory_read_invalid(mem_object_t* ctx, void* memory_buffer, uint16_t size, uint32_t offset);
uint16_t memory_write_invalid(mem_object_t* ctx, const void* memory_buffer, uint16_t size, uint32_t offset);
pull_error memory_flush_invalid(mem_object_t* ctx);
pull_error memory_close_invalid(mem_object_t* ctx);

#endif /* MEMORY_MOCK_H_ */
