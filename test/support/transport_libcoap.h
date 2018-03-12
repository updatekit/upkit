#ifndef TRANSPORT_LIBCOAP_H_
#define TRANSPORT_LIBCOAP_H_
#include "network/transport.h"
#include "common/error.h"
#include "common/logger.h"
#include "security/ecc.h"

#include <coap/coap.h>

#define COAP_DEFAULT_PORT 5683
#define COAPS_DEFAULT_PORT 5684

// To support blockwise requests the callback should be aware of the
// parameters used to perform the request, to create a new response with
// the correct data
typedef struct {
    uint8_t method;
    uint8_t enabled;
    const char* resource;
    const char* data;
    uint16_t length;
} blockwise_ctx_t;

/* Supported connection types. You can however define your own
 * connection type and pass it to the txp_init function, with
 * its own data. If you are implementing a connection type
 * defined here you can find some predefined structs in
 * security/secure_transport.h. */
typedef enum conn_type {
    CONN_UDP, // No connection data
    CONN_DTLS_PSK,
    CONN_DTLS_ECDH
} conn_type;

typedef struct dtls_psk_data_t {
    char* hint;
    uint8_t* key;
    size_t key_len;
} dtls_psk_data_t;

typedef struct dtls_ecdh_data_t {
    curve_t curve;
    uint8_t* priv_key;
    uint8_t* pub_key_x;
    uint8_t* pub_key_y;
    int (*verify_key) (txp_ctx* ctx, uint8_t key_size,
            const uint8_t* server_pub_x, const uint8_t* server_pub_y);
} dtls_ecdh_data_t;

typedef struct {
    callback cb;
    void* more;
    blockwise_ctx_t bctx;
} cb_ctx_t;

typedef struct txp_ctx_ {
    coap_context_t* coap_ctx;
    coap_session_t* coap_session;
    cb_ctx_t cb_ctx;
    void* conn_data;
    uint8_t loop;
} txp_ctx;

pull_error txp_init(txp_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* conn_data);

pull_error txp_on_data(txp_ctx* ctx, callback handler, void* more);

pull_error txp_request(txp_ctx* ctx, rest_method method, const char* resource, const char* data, uint16_t length);

pull_error txp_observe(txp_ctx* ctx, const char* resource, const char* token, uint8_t token_length);

void txp_end(txp_ctx* ctx);

#endif
