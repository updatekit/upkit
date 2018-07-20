#ifdef WITH_UPDATE_AGENT

#include <libpull_agents/update.h>

#include "platform_headers.h"

agent_msg_t update_agent(update_agent_config* cfg, update_agent_ctx_t* ctx) {

    // (1) Init transport interface
    PULL_BEGIN(EVENT_INIT);
    ctx->err = txp_init(&ctx->stxp, cfg->subscriber.endpoint, cfg->subscriber.port, cfg->subscriber.connection_type, cfg->subscriber.conn_data);
    if (ctx->err) {
        log_error(ctx->err, "Error initializing the transport interface\n");
        PULL_CONTINUE(EVENT_INIT_FAILURE, &ctx->err);
    }

    // (2) Subscribe to the server
    PULL_CONTINUE(EVENT_SUBSCRIBE, NULL);
    ctx->err = subscribe(&ctx->sctx, &ctx->stxp, cfg->subscriber.resource, &ctx->obj_t);
    if (ctx->err) {
        log_error(ctx->err, "Error subscribing to the server\n");
        PULL_CONTINUE(EVENT_SUBSCRIBE_FAILURE, &ctx->err);
    }

    // (3) Check if there are updates
    PULL_CONTINUE(EVENT_CHECKING_UPDATES, NULL);
    log_info("Checking for updates\n");
    while (!ctx->sctx.has_updates) {
        ctx->err = check_updates(&ctx->sctx, subscriber_cb);
        if (ctx->err) {
            log_error(ctx->err, "Error setting check udpates callback\n");
            PULL_CONTINUE(EVENT_CHECKING_UPDATES_RECOVER, &ctx->err);
            continue;
        }
        PULL_CONTINUE(EVENT_CHECKING_UPDATES_SEND, &(ctx->stxp));
        if (!ctx->sctx.has_updates) {
            PULL_CONTINUE(EVENT_CHECKING_UPDATES_TIMEOUT, NULL);
        }
    }
    log_info("An update is available\n");

    // (4) Searching slot
    PULL_CONTINUE(EVENT_SEARCHING_SLOT, NULL);
    uint16_t version;
    ctx->err = get_oldest_firmware(&ctx->id, &version, &ctx->obj_t, true, true);
    if (ctx->err) {
        log_error(ctx->err, "Error while getting the oldest slot\n");
        PULL_CONTINUE(EVENT_SEARCHING_SLOT_FAILURE, &ctx->err);
    }

    ctx->err = memory_open(&ctx->new_obj, ctx->id, WRITE_ALL);
    if (ctx->err) {
        log_error(ctx->err, "Error opening the memory_object\n");
        PULL_CONTINUE(EVENT_SEARCHING_SLOT_FAILURE_2, &ctx->err);
    }

    // (5) Connection to receiver server
    PULL_CONTINUE(EVENT_CONN_RECEIVER, NULL);
    static txp_ctx* rtxp;
    if (cfg->reuse_connection) {
        rtxp = &ctx->stxp;
    } else { 
        ctx->err = txp_init(&ctx->rtxp, cfg->receiver.endpoint,
                    cfg->receiver.port, cfg->receiver.connection_type, 
                    cfg->receiver.conn_data);
        if (ctx->err) {
            log_error(ctx->err, "Error while connecting to receiver server\n");
            PULL_CONTINUE(EVENT_CONN_RECEIVER_FAILURE, &ctx->err);
        }
        rtxp = &ctx->rtxp;
    }
    ctx->err = receiver_open(&ctx->rctx, rtxp, &cfg->identity, cfg->receiver.resource, &ctx->new_obj);
    if (ctx->err) {
        log_error(ctx->err, "Error opening the receiver\n");
        PULL_CONTINUE(EVENT_CONN_RECEIVER_FAILURE_2, &ctx->err);
    }

    // (5) Receiving update
    PULL_CONTINUE(EVENT_RECEIVE, NULL);
    while (!ctx->rctx.firmware_received) {
        ctx->err = receiver_chunk(&ctx->rctx);
        PULL_CONTINUE(EVENT_RECEIVE_SEND, rtxp);
        if (ctx->err) {
            log_error(ctx->err, "Error receiving chunk\n");
            PULL_CONTINUE(EVENT_RECEIVE_RECOVER, &ctx->err);
            continue;
        }
    }
    if (!cfg->reuse_connection) {
        txp_end(&ctx->rtxp);
    }
    ctx->err = receiver_close(&ctx->rctx); // Check this ctx->error

    // (6) Verify received firmware
    PULL_CONTINUE(EVENT_VERIFY, NULL);
    PULL_CONTINUE(EVENT_VERIFY_BEFORE, NULL);
    ctx->err = verify_object(&ctx->new_obj, cfg->df, cfg->vendor_x, cfg->vendor_y, cfg->ef, 
            cfg->buffer, cfg->buffer_size);
    PULL_CONTINUE(EVENT_VERIFY_AFTER, NULL);
    if (ctx->err) {
        log_error(ctx->err, "Verification failed\n");
        PULL_CONTINUE(EVENT_VERIFY_FAILURE, &ctx->err);
        ctx->err = invalidate_object(ctx->id, &ctx->obj_t);
        if (ctx->err) {
            log_error(ctx->err, "Error invalidating object\n");
            PULL_CONTINUE(EVENT_INVALIDATE_OBJECT_FAILURE, &ctx->err);
        }
    }

    // (7) Final
    PULL_CONTINUE(EVENT_FINAL, NULL);
    ctx->err = memory_close(&ctx->new_obj);
    ctx->err = unsubscribe(&ctx->sctx);
    txp_end(&ctx->stxp);
    PULL_FINISH(EVENT_APPLY);
}

#endif /* WITH_UPDATE_AGENT */
