#include "memory/memory.h"

pull_error memory_open_impl(mem_object* ctx, obj_id obj);
uint16_t memory_read_impl(mem_object* ctx, void* memory_buffer, uint16_t size, uint32_t offset);
uint16_t memory_write_impl(mem_object* ctx, const void* memory_buffer, uint16_t size, uint32_t offset);
pull_error memory_flush_impl(mem_object* ctx);
pull_error memory_close_impl(mem_object* ctx);

typedef struct {
    pull_error (*memory_open_impl) (mem_object*, obj_id);
    uint16_t (*memory_read_impl)(mem_object*, void*, uint16_t, uint32_t);
    uint16_t (*memory_write_impl)(mem_object*, const void*, uint16_t, uint32_t);
    pull_error (*memory_flush_impl)(mem_object*);
    pull_error (*memory_close_impl)(mem_object*);
} memory_mock_t;

memory_mock_t memory_mock = {
    .memory_open_impl = memory_open_impl,
    .memory_read_impl = memory_read_impl,
    .memory_write_impl = memory_write_impl,
    .memory_flush_impl = memory_flush_impl,
    memory_close_impl = memory_close_impl
};

inline pull_error memory_open(mem_object* ctx, obj_id obj) {
    return memory_mock->memory_open_impl(ctx, obj);
}

inline int memory_read(mem_object* ctx, void* memory_buffer, uint16_t size, uint32_t offset) {
    return memory_mock->memory_read_impl(ctx, memory_buffer, size, offset);
}

inline int memory_write(mem_object* ctx, const void* memory_buffer, uint16_t size, uint32_t offset) {
    return memory_mock->memory_write_impl(ctx, memory_buffer, size, offset);
}

inline pull_error memory_flush(mem_object* ctx) {
    return memory_mock->memory_flush_impl(ctx);
}

inline pull_error memory_close(mem_object* ctx) {
    return memory_mock->memory_close_impl(ctx);
}
