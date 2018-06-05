#include <libpull/memory.h>
#include <string.h>

pull_error memory_open_impl(mem_object_t* ctx, mem_id_t id, mem_mode_t mode) {
    PULL_ASSERT(ctx != NULL);
    memcpy(ctx, &flash_objects[id], sizeof(mem_object_t));
    ctx->mode = mode;

    if (ctx->fdescr->open != NULL && ctx->fdescr->open() != 0) {
        return MEMORY_OPEN_ERROR;
    }
    address_t offset;
    if (ctx->mode == WRITE_ALL) {
        for (offset = ctx->start; offset < ctx->end; offset += ctx->fdescr->page_size) {
            if (ctx->fdescr->erase(offset, ctx->fdescr->page_size) != 0) {
                log_debug("Error ereasing page %lu\n", offset/ctx->fdescr->page_size);
                return MEMORY_OPEN_ERROR;
            }
        }
        log_debug("Pages erased correctly\n");
    }
    return PULL_SUCCESS;
}

int memory_read_impl(mem_object_t* ctx, void* memory_buffer, size_t size, address_t offset) {
    PULL_ASSERT(ctx != NULL);
    PULL_ASSERT(ctx->start + offset + size < ctx->end);
    if (ctx->fdescr->read(memory_buffer, ctx->start+offset, size) == 0) {
        return size;
    }
    return -1;
}

int memory_write_impl(mem_object_t* ctx, const void* memory_buffer, size_t size, address_t offset) {
    PULL_ASSERT(ctx != NULL);
    PULL_ASSERT(ctx->start + offset + size < ctx->end_offset);
    if (ctx->mode == SEQUENTIAL_REWRITE && ((ctx->start + offset) % ctx->fdescr->page_size) == 0) {
        if (ctx->fdescr->erase(ctx->start + offset, ctx->fdescr->page_size) != 0) {
            log_error(MEMORY_ERASE_ERROR, "Error erasing page\n");
            return -1;
        }
    }
    if (ctx->fdescr->program(memory_buffer, ctx->start+offset, size) == 0) {
        return size;
    }
    return -1;
}

pull_error memory_close_impl(mem_object_t* ctx) {
    PULL_ASSERT(ctx != NULL);
    if (ctx->fdescr->close != NULL && ctx->fdescr->close() != 0) {
        return MEMORY_CLOSE_ERROR;
    }
    return PULL_SUCCESS;
}

