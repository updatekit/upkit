#include <libpull/network/subscriber.h>
#include <libpull/network/connection_interface.h>
#include <libpull/network/async_interface.h>
#include <libpull/common.h>

static void request_cb(pull_error err, const char* data, int len, void* more) {
    log_debug("cb: data received\n");

    update_agent_ctx_t* ctx = (update_agent_ctx_t*) more;
    if (err || data == NULL || len != sizeof(uint16_t)) {
        log_error(err, "Error in the subscriber callback\n");
        break_loop(ctx->conn);
        ctx->err = err;
        return;
    }
    ctx->err = fsm(ctx->fsmc, CMD_SUBSCRIBE, data, len);
    if (ctx->err) {
        log_error(ctx->err, "Error in the subscriber callback\n");
        break_loop(ctx->conn);
        return;
    }
}

pull_error request(subscriber_ctx* ctx, conn_ctx* conn, const char* resource, fsm_ctx_t* fsm, void* data, size_t len) {
    // Set the finite state machine context
    {
        ctx->fsmc = fsmc;
        ctx->conn = conn;
        ctx->resource = resource;
    }

    // Perform the first request
    err = conn_request(conn, GET, resource, data, len);
    if (err) {
        log_debug("Error sending the request\n");
        return SUBSCRIBER_CHECK_ERROR;
    }
    return PULL_SUCCESS;
}
