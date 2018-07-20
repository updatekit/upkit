#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/network/writer.h>
#include <string.h>

#ifdef BUFFERED_WRITER

pull_error writer_open(writer_ctx_t* ctx, mem_object_t* obj, validate_mt cb, void* user_data) {
    ctx->obj = obj;
    ctx->received = 0;
    ctx->expected = 0;
    ctx->manifest_received = false;
    ctx->validate_cb = cb;
    ctx->user_data = user_data;

    ctx->buffer_full = 0;
    ctx->buffer_ptr = 0;
    return PULL_SUCCESS;
}

static pull_error buffer_flush(writer_ctx_t* ctx) {
    pull_error err = memory_write(ctx->obj, ctx->buffer, ctx->buffer_full, ctx->buffer_ptr);
    ctx->buffer_ptr += WRITER_BUFFER_LEN;
    ctx->buffer_full = 0;
    return err;
}

static pull_error buffer_manage(writer_ctx_t* ctx, const uint8_t* data, size_t len, address_t received) {
    size_t elen = len;
    uint8_t* chunk = data;
    pull_error err = PULL_SUCCESS;
    if (ctx->buffer_full + len >= WRITER_BUFFER_LEN) {
        chunk = data + (WRITER_BUFFER_LEN - ctx->buffer_full);
        elen = len - (chunk - data);
        // (1) Copy part of the received data to the buffer
        memcpy(&ctx->buffer[ctx->buffer_full], data, chunk-data);
        ctx->buffer_full += chunk-data;
        // (2) Flush the buffer
        err = buffer_flush(ctx);
    }
    
    if (ctx->buffer_full + elen < WRITER_BUFFER_LEN) {
        memcpy(&ctx->buffer[ctx->buffer_full], chunk, elen);
    }
    return err;
}

pull_error writer_chunk(writer_ctx_t* ctx, const char* data, uint32_t len) {
    if (ctx->manifest_received && ctx->received+len > ctx->expected) {
        return INVALID_SIZE_ERROR;    
    } else if (ctx->manifest_received && ctx->received+len == ctx->expected) {
        buffer_flush(ctx);
    }
    if (buffer_manage(ctx, (const uint8_t*) data, len, ctx->received) != PULL_SUCCESS) {
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

#endif /* BUFFERED_WRITER */
