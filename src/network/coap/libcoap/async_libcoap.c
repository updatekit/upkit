#ifdef WITH_CONNECTION_LIBCOAP
#include "async_libcoap.h"

#include <coap/coap.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/* This is a blocking function, it will return when the
 * message has been received or the timeout is exceeded */
void loop_once(txp_ctx* ctx, uint32_t timeout) {
    coap_run_once(ctx->coap_ctx, timeout);
}

void loop(txp_ctx* ctx, uint32_t timeout) {
    ctx->loop = 1;
    while(ctx->loop) {
        coap_run_once(ctx->coap_ctx, timeout);
    }
    log_debug("Loop end\n");
}

void break_loop(txp_ctx* ctx) {
    ctx->loop = 0;
}

#endif /* WITH_CONNECTION_LIBCOAP */
