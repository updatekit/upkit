#ifdef WITH_CONNECTION_LIBCOAP

#include <libpull_network/coap/async_libcoap.h>

#include <coap/coap.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "platform_headers.h"

/* This is a blocking function, it will return when the
 * message has been received or the timeout is exceeded */
void loop_once(conn_ctx* ctx, uint32_t timeout) {
    coap_run_once(ctx->coap_ctx, timeout);
}

void loop(conn_ctx* ctx, uint32_t timeout) {
    ctx->loop = 1;
    while(ctx->loop) {
        log_debug("Starting loop\n");
        int ret = coap_run_once(ctx->coap_ctx, timeout);
        if (ret > timeout) {
            log_debug("Timeout received\n");
            return;
        }
    }
    log_debug("Loop end\n");
}

void break_loop(conn_ctx* ctx) {
    ctx->loop = 0;
}

#endif /* WITH_CONNECTION_LIBCOAP */
