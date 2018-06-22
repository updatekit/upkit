#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/network/writer.h>
#include <string.h>

pull_error writer_open(writer_ctx_t* ctx, mem_object_t* obj, validate_mt cb, void* user_data) {
    ctx->obj = obj;
    ctx->received = 0;
    ctx->expected = 0;
    ctx->manifest_received = false;
    ctx->validate_cb = cb;
    ctx->user_data = user_data;
    return PULL_SUCCESS;
}

pull_error writer_chunk(writer_ctx_t* ctx, const char* data, uint32_t len) {
    if (ctx->manifest_received && ctx->received+len > ctx->expected) {
        return INVALID_SIZE_ERROR;    
    }
    if (memory_write(ctx->obj, (const uint8_t*) data, len, ctx->received) != len) {
        log_error(MEMORY_WRITE_ERROR, "Failure writing the received data\n");
        return MEMORY_WRITE_ERROR;
    }
    if (!ctx->manifest_received) {
        log_debug("Manifest still not received\n");
        int16_t missing = sizeof(manifest_t)-ctx->received;
        if (missing <= 0) {
            if (ctx->validate_cb && ctx->validate_cb(&ctx->mt, ctx->user_data)) {
                    return INVALID_MANIFEST_ERROR;
            }
            ctx->expected = get_size(&ctx->mt)+get_offset(&ctx->mt);
            ctx->manifest_received = 1;
        } else {
            uint8_t* mt_ptr = (uint8_t*) &ctx->mt;
            memcpy(mt_ptr+ctx->received, data, ctx->received+len<=sizeof(manifest_t)? len: missing);
        }
    }
    ctx->received+=len;
    log_info("Received %u bytes. Expected %u bytes\r", ctx->received, ctx->expected);
    return PULL_SUCCESS;
}

pull_error writer_close(writer_ctx_t* ctx) {
    return PULL_SUCCESS;
}
