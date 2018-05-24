#include "common/libpull.h"
#include "network/receiver.h"
#include "network/subscriber.h"
#include "network/async.h"
#include "security/ecc.h"
#include "security/sha256.h"
#include "security/verifier.h"
#include "memory/memory_objects.h"
#include "memory/memory.h"
#include "memory/manifest.h"
#include "agents/update.h"

#include "platform_headers.h"

agent_t update_agent(update_agent_config* cfg, update_agent_ctx_t* ctx) {

    // (1) Init transport interface
    PULL_BEGIN(STATE_INIT);
    ctx->err = txp_init(&ctx->stxp, cfg->subscriber.endpoint, cfg->subscriber.port, cfg->subscriber.connection_type, cfg->subscriber.conn_data);
    if (ctx->err) {
        log_error(ctx->err, "Error initializing the transport interface\n");
        PULL_FAILURE(ctx->err);
    }

    // (2) Subscribe to the server
    PULL_CONTINUE(STATE_SUBSCRIBE);
    ctx->err = subscribe(&ctx->sctx, &ctx->stxp, cfg->subscriber.resource, &ctx->obj_t);
    if (ctx->err) {
        log_error(ctx->err, "Error subscribing to the server\n");
        PULL_FAILURE(ctx->err);
    }

    // (3) Check if there are updates
    PULL_CONTINUE(STATE_CHECKING_UPDATES);
    log_info("Checking for updates\n");
    while (!ctx->sctx.has_updates) {
        ctx->err = check_updates(&ctx->sctx, subscriber_cb);
        if (ctx->err) {
            log_error(ctx->err, "Error setting check udpates callback\n");
            PULL_RECOVER(STATE_CHECKING_UPDATES_FAILURE, ctx->err);
            continue;
        }
        PULL_SEND(STATE_CHECKING_UPDATES_SEND);
    }
    log_info("An update is available\n");

    // (4) Searching slot
    PULL_CONTINUE(STATE_SEARCHING_SLOT);
    uint16_t version;
    ctx->err = get_oldest_firmware(&ctx->id, &version, &ctx->obj_t);
    if (ctx->err) {
        log_error(ctx->err, "Error while getting the oldest slot\n");
        PULL_FAILURE(ctx->err);
    }
    ctx->err = memory_open(&ctx->new_obj, ctx->id, WRITE_ALL);
    if (ctx->err) {
        log_error(ctx->err, "Error opening the memory_object\n");
        PULL_FAILURE(ctx->err);
    }

    // (5) Connection to receiver server
    PULL_CONTINUE(STATE_CONN_RECEIVER);
    if (cfg->reuse_connection) {
        ctx->rtxp = ctx->stxp;
    } else { 
        ctx->err = txp_init(&ctx->rtxp, cfg->receiver.endpoint,
                    cfg->receiver.port, cfg->receiver.connection_type, 
                    cfg->receiver.conn_data);
        if (ctx->err) {
            log_error(ctx->err, "Error while connecting to receiver server\n");
            PULL_FAILURE(ctx->err);
        }
    }
    ctx->err = receiver_open(&ctx->rctx, &ctx->rtxp, cfg->identity, cfg->receiver.resource, &ctx->new_obj);
    if (ctx->err) {
        log_error(ctx->err, "Error opening the receiver\n");
        PULL_FAILURE(ctx->err);
    }

    // (5) Receiving update
    PULL_CONTINUE(STATE_RECEIVE);
    while (!ctx->rctx.firmware_received) {
        ctx->err = receiver_chunk(&ctx->rctx);
        if (ctx->err) {
            log_error(ctx->err, "Error receiving chunk\n");
            PULL_RECOVER(STATE_RECEIVE_FAILURE, ctx->err);
            continue;
        }
        PULL_SEND(STATE_RECEIVE_SEND);
    }
    txp_end(&ctx->rtxp);
    ctx->err = receiver_close(&ctx->rctx); // Check this ctx->error

    // (6) Verify received firmware
    PULL_CONTINUE(STATE_VERIFY);
    PULL_CONTINUE(STATE_VERIFY_BEFORE);
    ctx->err = verify_object(&ctx->new_obj, cfg->df, cfg->vendor_x, cfg->vendor_y, cfg->ef, 
            cfg->buffer, cfg->buffer_size);
    PULL_CONTINUE(STATE_VERIFY_AFTER);
    if (ctx->err) {
        log_error(ctx->err, "Verification failed\n");
        PULL_FAILURE(ctx->err);
    }

    // (7) Final
    PULL_CONTINUE(STATE_FINAL);
    ctx->err = memory_close(&ctx->new_obj);
    ctx->err = unsubscribe(&ctx->sctx);
    txp_end(&ctx->stxp);
    PULL_FINISH(STATE_APPLY);
}

