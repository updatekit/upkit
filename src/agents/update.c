#ifdef WITH_UPDATE_AGENT

#include <libpull_agents/update.h>

#include "platform_headers.h"


// (X) Timer
#ifdef ZEPHYR
s64_t time_stamp;
s32_t milliseconds_spent;
#endif

agent_event_t update_agent(update_agent_config_t* cfg, update_agent_ctx_t* ctx, void** event_data) {

    // (X) Init timer
    #ifdef ZEPHYR
    k_enable_sys_clock_always_on();
    time_stamp = k_uptime_get();
    #endif

    // (1) Init the connection interface
    PULL_BEGIN(EVENT_INIT);
    ctx->err = conn_init(&ctx->conn, cfg->conn->endpoint, cfg->conn->port, cfg->conn->connection_type, cfg->conn->conn_data);
    if (ctx->err) {
        log_error(ctx->err, "Error initializing the connection interface\n");
        PULL_RETURN(EVENT_INIT_FAILURE, &ctx->err);
    }

    // (2) Init the finite state machine
    ctx->err = fsm_init(&ctx->fsmc, cfg->safestore, &ctx->obj_t, &ctx->old_obj);
    if (ctx->err) {
        log_err(ctx->err, "Error initializing the finite state machine\n");
        PULL_RETURN(EVENT_INIT_FAILURE_2, &ctx->err);
    }

    // (3) Set callback routing data to fsm
    ctx->reqc.fsmc = &ctx->fsmc;
    ctx->reqc.conn = &ctx->conn;

    ctx->err = conn_on_data(&ctx->conn, request_cb, &ctx->reqc);
    if (ctx->err) {
        log_error(ctx->err, "Error setting the callback");
        PULL_RETURN(EVENT_INIT_FAILURE_3, &ctx->err);
    }

    // (4) Subscribe to the server
    PULL_CONTINUE(EVENT_SUBSCRIBE, NULL);
    do {
        ctx->err = conn_request(&ctx->conn, GET, "/version", NULL, 0);
        if (ctx->err) {
            log_debug("Error sending the request\n");
            PULL_RETURN(EVENT_SUBSCRIBE_FAILURE, &ctx->err);
        }
        PULL_SEND(EVENT_SUBSCRIBE_SEND, &(ctx->conn));
        if (ctx->reqc.err) {
            log_err(ctx->reqc.err, "Error in the callback\n");
            PULL_RETURN(EVENT_SUBSCRIBE_FAILURE_2, &ctx->reqc.err);
        }
    } while (ctx->fsmc.state != STATE_START_UPDATE);

    // (5) The fsm returns the data to be passed to the update server
    PULL_CONTINUE(EVENT_GET_MSG, NULL);
    ctx->err = fsm(&ctx->fsmc, (uint8_t*) &ctx->msg, sizeof(receiver_msg_t));
    if (ctx->err) {
        log_err(ctx->err, "Invalid identity");
        PULL_RETURN(EVENT_GET_MSG_FAILURE, &ctx->err);
    }

    // (6) Perform the request passing the data
    PULL_CONTINUE(EVENT_RECEIVE, NULL);
    do {
        update_receiver_msg(&ctx->fsmc, &ctx->msg);
        ctx->err = conn_request(&ctx->conn, GET_BLOCKWISE2, "/firmware", (const char*)&ctx->msg, sizeof(receiver_msg_t));
        if (ctx->err) {
            log_debug("Error sending the request\n");
            PULL_RETURN(EVENT_RECEIVER_FAILURE, &ctx->err);
        }
        PULL_SEND(EVENT_RECEIVE_SEND, &(ctx->conn));
        if (ctx->reqc.err) {
            log_err(ctx->reqc.err, "Error in the callback\n");
            PULL_RETURN(EVENT_RECEIVER_FAILURE_2, &ctx->reqc.err);
        } else {
            PULL_CONTINUE(EVENT_SUBSCRIBE_TIMEOUT, NULL);
        }
    } while (ctx->fsmc.state == STATE_RECEIVE_FIRMWARE);
    conn_end(&ctx->conn);

    if (ctx->fsmc.state != STATE_REBOOT) {
        log_err(ctx->reqc.err, "Validation Error");
        // Validation error
    }
    PULL_FINISH(EVENT_APPLY);
}

#endif /* WITH_UPDATE_AGENT */
