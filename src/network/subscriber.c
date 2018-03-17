#include "network/subscriber.h"
#include "network/transport.h"
#include "memory/memory_objects.h"
#include "common/libpull.h"
#include "network/async.h"

pull_error subscribe(subscriber_ctx* ctx, txp_ctx* txp, const char* resource, mem_object* obj_t) {
    pull_error err;
    ctx->txp = txp;
    ctx->resource = resource;
    obj_id id;
    log_debug("Getting the newest firmware from memory\n");
    err = get_newest_firmware(&id, &ctx->current_version, obj_t);
    if (err) {
        log_error(err, "Impossible to get newest firmware\n");
        return SUBSCRIBE_ERROR;
    }
    printf("the id of the newest firmware is %d\n", id);
    ctx->has_updates = 0;
    return PULL_SUCCESS;
}

void subscriber_cb(pull_error err, const char* data, int len, void* more) {
    subscriber_ctx* ctx = (subscriber_ctx*) more;
    log_debug("subsriber_cb: message received\n");
    if (data == NULL || len != sizeof(uint16_t)) {
        log_error(SUBSCRIBE_ERROR, "Received data invalid\n");
        return;
    }
    uint16_t provisioning_version = *((uint16_t*)data);
    log_info("Latest_version: %4x Provisioning version %4x\n",
            ctx->current_version, provisioning_version);
    if (provisioning_version > ctx->current_version) {
        log_info("An update is available\n");
        ctx->has_updates = 1;
    }
}

pull_error check_updates(subscriber_ctx* ctx, callback cb) {
    pull_error err = txp_on_data(ctx->txp, cb, ctx);
    if (err) {
        log_error(err, "Error setting the callback");
        return SUBSCRIBE_ERROR;
    }
    err = txp_request(ctx->txp, GET, ctx->resource, NULL, 0);
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
