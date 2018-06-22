#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/network/writer.h>
#include <string.h>

pull_error valid_cb(manifest_t* mt, void* user_data) {
    log_debug("Manifest received\n");
    print_manifest(mt);
    /*identity_t i = *(identity_t*) user_data;

    if (validate_identity(i, get_identity(mt)) != PULL_SUCCESS) {
        log_debug("Received invalid identity\n");
        return INVALID_IDENTITY_ERROR;
    }*/
    return PULL_SUCCESS;
}

pull_error writer_open(writer_ctx_t* ctx, mem_object_t* obj, validate_mt v, void* user_data) {
    ctx->obj = obj;
    ctx->received = 0;
    ctx->expected = 0;
    ctx->manifest_received = false;
   /* ctx->validate = v; */
    ctx->validate = valid_cb;
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
            if (ctx->validate && ctx->validate(&ctx->mt, ctx->user_data)) {
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
