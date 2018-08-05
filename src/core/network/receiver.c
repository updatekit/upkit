#include <libpull/network/receiver.h>
#include <libpull/network/writer.h>
#include <libpull/network/async_interface.h>
#include <libpull/memory/memory_objects.h>
#include <libpull/memory/memory_interface.h>
#include <libpull/common.h>

#define MAX_RECEIVER_ERRORS 10

pull_error validate_manifest(manifest_t* mt, void* validate_data) {
    log_debug("Manifest received\n");
    identity_t i = *(identity_t*) validate_data;
    print_manifest(mt);

    if (validate_identity(i, get_identity(mt)) != PULL_SUCCESS) {
        log_debug("Received invalid identity\n");
        return INVALID_IDENTITY_ERROR;
    }
    return PULL_SUCCESS;
}

static void handler(pull_error conn_err, const char* data, int len, void* more) {
    receiver_ctx* ctx = (receiver_ctx*) more;
    if (conn_err) {
        log_error(conn_err, "Network failure\n");
        ctx->err = NETWORK_ERROR; 
        break_loop(ctx->conn);
        return;
    }
    if (len <= 0 || data == NULL) {
        log_error(NETWORK_ERROR, "The received lenght (%d) is invalid\n", len);
        ctx->err = NETWORK_ERROR; 
        break_loop(ctx->conn);
        return;
    }
    ctx->err = writer_chunk(&ctx->wctx, data, len);
    if (ctx->err) {
        break_loop(ctx->conn);
        return;
    }
    if (ctx->wctx.received == ctx->wctx.expected) {
        ctx->firmware_received = 1;
        break_loop(ctx->conn);
    }
}

pull_error receiver_open(receiver_ctx* ctx, conn_ctx* conn, identity_t* identity,
        const char* resource, mem_object_t* obj) {
    memset(ctx, 0, sizeof(receiver_ctx));
    ctx->conn = conn;
    ctx->resource = resource;
    ctx->identity = identity;
    writer_open(&ctx->wctx, obj, validate_manifest, identity);
    return PULL_SUCCESS;
}

pull_error receiver_chunk(receiver_ctx* ctx) {
    pull_error err = conn_on_data(ctx->conn, handler, ctx);
    if (err) {
        log_error(err, "Failure setting receiver callback\n");
        return RECEIVER_CHUNK_ERROR;
    }
    switch(ctx->err) {
        // Recoverable errors
        case MEMORY_WRITE_ERROR:
        case NETWORK_ERROR:
            if (++ctx->num_err >= MAX_RECEIVER_ERRORS) {
                return ctx->err;
            }
            ctx->err = PULL_SUCCESS;
            break;
            // Not recoverable error. The upgrade process should start again
        case INVALID_SIZE_ERROR:
            return ctx->err;
        default: /* ignore all the other cases */ 
            break;
    }

    ctx->msg.msg_version = MESSAGE_VERSION;
    ctx->msg.offset = ctx->wctx.received;

    ctx->msg.udid = ctx->identity->udid;
    ctx->msg.random = ctx->identity->random;

    err = conn_request(ctx->conn, GET_BLOCKWISE2, ctx->resource, (const char*) &(ctx->msg), sizeof(receiver_msg_t));
    if (err) {
        log_error(err, "Failure setting receiver request\n");
        return RECEIVER_CHUNK_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error receiver_close(receiver_ctx* ctx) {
    writer_close(&ctx->wctx);
    return PULL_SUCCESS;
}
