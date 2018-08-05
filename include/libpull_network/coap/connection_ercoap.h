#ifndef CONNECTION_ERCOAP_H_
#define CONNECTION_ERCOAP_H_

#ifdef WITH_CONNECTION_ERCOAP

#include <libpull/common.h>
#include <libpull/network.h>

#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"

#define COAP_DEFAULT_PORT 5683
#define COAPS_DEFAULT_PORT 5684

typedef struct conn_ctx {
    callback cb;
    void* more;
    coap_endpoint_t endpoint;
    coap_message_t request;
    coap_request_state_t request_state;
} conn_ctx;

void ercoap_handler(coap_message_t* response);

// This macro takes a conn_ctx and sets the correct values to send a message
// with er-coap. The need of rewriting this macro is because I moved the
// request state to the conn_ctx to make sure we are able to block the loop
// in case of an error is detected in the callback
#define COAP_SEND(_ctx_) \
    {\
    PT_SPAWN(process_pt, &(_ctx_->request_state.pt), \
            coap_blocking_request(&_ctx_->request_state, ev, \
                                            &_ctx_->endpoint, \
                        &(_ctx_->request), ercoap_handler)); \
    };

pull_error conn_init(conn_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* data);

pull_error conn_on_data(conn_ctx* ctx, callback handler, void* more);

pull_error conn_request(conn_ctx* ctx, rest_method method, const char* resource, const char* data, uint16_t length);

void break_loop(conn_ctx* ctx);

pull_error conn_observe(conn_ctx* ctx, const char* resource, const char* token, uint8_t token_length);

void conn_end(conn_ctx* ctx);

#endif /* WITH_CONNECTION_ERCOAP */
#endif /* CONNECTION_ERCOAP_H_ */
