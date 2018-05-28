#include <libpull/memory/memory_interface.h>

pull_error memory_open_impl(mem_object_t* ctx, mem_id_t id, mem_mode_t mode);
uint16_t memory_read_impl(mem_object_t* ctx, void* memory_buffer, size_t size, address_t offset);
uint16_t memory_write_impl(mem_object_t* ctx, const void* memory_buffer, size_t size, address_t offset);
pull_error memory_flush_impl(mem_object_t* ctx);
pull_error memory_close_impl(mem_object_t* ctx);

inline pull_error memory_open(mem_object_t* ctx, mem_id_t id, mem_mode_t mode) {
    return memory_open_impl(ctx, id, mode);
}

inline int memory_read(mem_object_t* ctx, void* memory_buffer, size_t size, address_t offset) {
    return memory_read_impl(ctx, memory_buffer, size, offset);
}

inline int memory_write(mem_object_t* ctx, const void* memory_buffer, size_t size, address_t offset) {
    return memory_write_impl(ctx, memory_buffer, size, offset);
}

inline pull_error memory_flush(mem_object_t* ctx) {
    return memory_flush_impl(ctx);
}

inline pull_error memory_close(mem_object_t* ctx) {
    return memory_close_impl(ctx);
}
