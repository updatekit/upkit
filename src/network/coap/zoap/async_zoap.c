#ifdef WITH_CONNECTION_ZOAP

#include <libpull/common.h>
#include <libpull/network.h>

#include "platform_headers.h"

/* This is a blocking function, it will return when the
 * message has been received or the timeout is exceeded */
void loop_once(txp_ctx* ctx, uint32_t timeout) {
    net_context_recv(ctx->net_ctx, udp_receive, timeout, ctx);
}

void loop(txp_ctx* ctx, uint32_t timeout) {
    ctx->loop = 1;
    do {
        if (net_context_recv(ctx->net_ctx, udp_receive, timeout, ctx) != 0) {
            log_debug("received a timeout: break loop\n");
            break;
        }
        /* Find a way to receive a timeout error */
    } while(ctx->loop);
}

void break_loop(txp_ctx* ctx) {
    ctx->loop = 0;
}

#endif /* WITH_CONNECTION_ZOAP */
