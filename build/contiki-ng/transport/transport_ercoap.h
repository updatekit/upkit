#ifndef _TRANSPORT_ERCOAP_H
#define _TRANSPORT_ERCOAP_H

#include "common/callback.h"
#include "network/transport.h"

#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"

#define COAP_DEFAULT_PORT 5683
#define COAPS_DEFAULT_PORT 5684

struct txp_ctx_ {
    callback cb;
    void* more;
    coap_endpoint_t endpoint;
    coap_message_t request;
    coap_request_state_t request_state;
    uint16_t port;
};

void ercoap_handler(coap_message_t* response);

// This macro takes a txp_ctx and sets the correct values to send a message
// with er-coap. The need of rewriting this macro is because I moved the
// request state to the txp_ctx to make sure we are able to block the loop
// in case of an error is detected in the callback
#define COAP_SEND(_ctx_) \
    {\
    PT_SPAWN(process_pt, &(_ctx_.request_state.pt), \
            coap_blocking_request(&_ctx_.request_state, ev, \
                                            &_ctx_.endpoint, \
                        &(_ctx_.request), ercoap_handler)); \
    };

typedef enum conn_type {
    TXP_UDP, // No connection data
    TXP_DTLS_PSK,
    TXP_DTLS_ECDSA
} conn_type;

pull_error txp_init(txp_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* data);

pull_error txp_on_data(txp_ctx* ctx, callback handler, void* more);

pull_error txp_request(txp_ctx* ctx, rest_method method, const char* resource, const char* data, uint16_t length);

void break_loop(txp_ctx* ctx);

pull_error txp_observe(txp_ctx* ctx, const char* resource, const char* token, uint8_t token_length);

void txp_end(txp_ctx* ctx);

#endif // _TRANSPORT_ERCOAP_H
