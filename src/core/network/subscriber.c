#include <libpull/network/subscriber.h>
#include <libpull/network/connection_interface.h>
#include <libpull/network/async_interface.h>
#include <libpull/memory/memory_objects.h>
#include <libpull/common.h>


pull_error subscribe(subscriber_ctx* ctx, conn_ctx* conn, const char* resource, mem_object_t* obj_t) {
    pull_error err;
    ctx->conn = conn;
    ctx->resource = resource;
    mem_id_t id;
    log_debug("Getting the newest firmware from memory\n");
    err = get_newest_firmware(&id, &ctx->current_version, obj_t, false);
    if (err) {
        log_error(err, "Impossible to get newest firmware\n");
        return SUBSCRIBE_ERROR;
    }
    log_debug("the id of the newest firmware is %d\n", id);
    ctx->has_updates = 0;
    return PULL_SUCCESS;
}

void subscriber_cb(pull_error err, const char* data, int len, void* more) {
    subscriber_ctx* ctx = (subscriber_ctx*) more;
    log_debug("subsriber_cb: message received\n");
    if (data == NULL || len != sizeof(uint16_t)) {
        log_error(SUBSCRIBE_ERROR, "Received invalid data\n");
        break_loop(ctx->conn);
        return;
    }
    uint16_t provisioning_version = *((uint16_t*)data);
    log_debug("Latest_version: %4x Provisioning version %4x\n",
            ctx->current_version, provisioning_version);
    if (provisioning_version > ctx->current_version) {
        log_debug("An update is available\n");
        ctx->has_updates = 1;
        break_loop(ctx->conn);
        return;
    }
}

pull_error check_updates(subscriber_ctx* ctx, callback cb) {
    pull_error err = conn_on_data(ctx->conn, cb, ctx);
    if (err) {
        log_error(err, "Error setting the callback");
        return SUBSCRIBE_ERROR;
    }
    err = conn_request(ctx->conn, GET, ctx->resource, NULL, 0);
    if (err) {
        log_debug("Error creating the request\n");
        return SUBSCRIBER_CHECK_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error unsubscribe(subscriber_ctx* ctx) {
    // Do nothing for now
    return PULL_SUCCESS;
}
