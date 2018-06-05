#include <libpull/common/error.h>
#include <libpull/memory/memory_interface.h>

#include "memory_mock.h"

pull_error memory_open_impl(mem_object_t* ctx, mem_id_t obj, mem_mode_t);
uint16_t memory_read_impl(mem_object_t* ctx, void* memory_buffer, uint16_t size, uint32_t offset);
uint16_t memory_write_impl(mem_object_t* ctx, const void* memory_buffer, uint16_t size, uint32_t offset);
pull_error memory_close_impl(mem_object_t* ctx);

memory_mock_t memory_mock = {
    .memory_open_impl = memory_open_impl,
    .memory_read_impl = memory_read_impl,
    .memory_write_impl = memory_write_impl,
    .memory_close_impl = memory_close_impl
};

void memory_mock_restore() {
    memory_mock.memory_open_impl = memory_open_impl;
    memory_mock.memory_read_impl = memory_read_impl;
    memory_mock.memory_write_impl = memory_write_impl;
    memory_mock.memory_close_impl = memory_close_impl;
}

inline pull_error memory_open(mem_object_t* ctx, mem_id_t obj, mem_mode_t mode) {
    return memory_mock.memory_open_impl(ctx, obj, mode);
}

inline int memory_read(mem_object_t* ctx, void* memory_buffer, size_t size, address_t offset) {
    return memory_mock.memory_read_impl(ctx, memory_buffer, size, offset);
}

inline int memory_write(mem_object_t* ctx, const void* memory_buffer, size_t size, address_t offset) {
    return memory_mock.memory_write_impl(ctx, memory_buffer, size, offset);
}

inline pull_error memory_close(mem_object_t* ctx) {
    return memory_mock.memory_close_impl(ctx);
}

pull_error memory_open_invalid(mem_object_t* ctx, mem_id_t obj, mem_mode_t mode) {
    return GENERIC_ERROR;
}
uint16_t memory_read_invalid(mem_object_t* ctx, void* memory_buffer, uint16_t size, uint32_t offset) {
    return 0;
}
uint16_t memory_write_invalid(mem_object_t* ctx, const void* memory_buffer, uint16_t size, uint32_t offset) {
    return 0;
}

pull_error memory_close_invalid(mem_object_t* ctx) {
    return GENERIC_ERROR;
}
