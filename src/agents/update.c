#ifdef WITH_UPDATE_AGENT

#include <libpull_agents/update.h>

#include "platform_headers.h"

agent_event_t update_agent(update_agent_config* cfg, update_agent_ctx_t* ctx, void** event_data) {

    // (1) Init the connection interface
    PULL_BEGIN(EVENT_INIT);
    ctx->err = conn_init(&ctx->conn, cfg->subscriber.endpoint, cfg->subscriber.port, cfg->subscriber.connection_type, cfg->subscriber.conn_data);
    if (ctx->err) {
        log_error(ctx->err, "Error initializing the connection interface\n");
        PULL_RETURN(EVENT_INIT_FAILURE, &ctx->err);
    }

    // (2) Init the finite state machine
    // TODO

    // (3) Set callback
    ctx->err = conn_on_data(ctx->conn, fsm_cb, ctx);
    if (err) {
        log_error(err, "Error setting the callback");
        return CALLBACK_ERROR;
    }

    // (4) Subscribe to the server
    PULL_CONTINUE(EVENT_CHECK_UPDATES, NULL);
    do {
        ctx->err = conn_request(ctx->conn, GET, cfg->subscriber.resource, NULL, 0);
        if (ctx->err) {
            log_debug("Error sending the request\n");
            return SUBSCRIBER_CHECK_ERROR;
        }
        PULL_SEND(EVENT_CHECKING_UPDATES_SEND, &(ctx->conn));
        if (ctx->conn.err) {
            // Error in the callback! Handle and subscribe again
        }
    } while (fsm->state != STATE_START_UPDATE);

    // (5) The fsm returns the data to be passed to the update server
    receiver_msg msg;
    err = fsm(ctx->fsm, &msg, sizeof(msg));
    if (err) {
        log_err(err, "Invalid identity");
        return err;
    }

    // (6) Perform the request passing the data
    PULL_CONTINUE(EVENT_RECEIVER, NULL);
    do {
        ctx->err = conn_request(ctx->conn, GET, cfg->receiver.resource, &msg, sizeof(msg));
        if (ctx->err) {
            log_debug("Error sending the request\n");
            return SUBSCRIBER_CHECK_ERROR;
        }
        PULL_SEND(EVENT_CHECKING_UPDATES_SEND, &(ctx->conn));
        if (ctx->conn.err) {
            // Error in the callback! Handle and subscribe again
        }
    } while (ctx->fsm->state == STATE_RECEIVE_FIRMWARE);
    conn_end(&ctx->conn);

    if (ctx->fsm->state != STATE_REBOOT) {
        // Validation error
    }
    PULL_FINISH(EVENT_APPLY);
}

#endif /* WITH_UPDATE_AGENT */
