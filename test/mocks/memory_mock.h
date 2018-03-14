#ifndef MEMORY_MOCK_H_
#define MEMORY_MOCK_H_

#include "memory/memory.h"

typedef struct {
    pull_error (*memory_open_impl) (mem_object*, obj_id);
    uint16_t (*memory_read_impl)(mem_object*, void*, uint16_t, uint32_t);
    uint16_t (*memory_write_impl)(mem_object*, const void*, uint16_t, uint32_t);
    pull_error (*memory_flush_impl)(mem_object*);
    pull_error (*memory_close_impl)(mem_object*);
} memory_mock_t;

extern memory_mock_t memory_mock;

void memory_mock_restore();

pull_error memory_open_invalid(mem_object* ctx, obj_id obj);
uint16_t memory_read_invalid(mem_object* ctx, void* memory_buffer, uint16_t size, uint32_t offset);
uint16_t memory_write_invalid(mem_object* ctx, const void* memory_buffer, uint16_t size, uint32_t offset);
pull_error memory_flush_invalid(mem_object* ctx);
pull_error memory_close_invalid(mem_object* ctx);

#endif /* MEMORY_MOCK_H_ */
