#include <stdio.h>

#include "async.h"
#include "connection.h"

/* This is a blocking function, it will return when the
 * message has been received or the timeout is exceeded */
void loop_once(txp_ctx* ctx, uint32_t timeout) {
    if (net_context_recv(ctx->net_ctx, udp_receive, timeout, ctx) != 0) {
        log_debug("Error receiving data\n");
        // TODO set the error in ctx->err 
        break_loop(ctx);
    }
}

void loop(txp_ctx* ctx, uint32_t timeout) {
    ctx->loop = 1;
    do {
        log_debug("Starting loop\n");
        loop_once(ctx, timeout);
        /* Find a way to receive a timeout error */
    } while(ctx->loop);
    log_debug("Loop end\n");
}

void break_loop(txp_ctx* ctx) {
    ctx->loop = 0;
}
