#ifndef CONNECTION_LIBCOAP_H_
#define CONNECTION_LIBCOAP_H_

#ifdef WITH_CONNECTION_LIBCOAP_RIOT

#include <libpull/common.h>
#include <libpull/network/connection_interface.h>
#include <libpull/security/ecc.h>

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

typedef struct dtls_psk_data_t {
    char* hint;
    uint8_t* key;
    size_t key_len;
} dtls_psk_data_t;

typedef struct dtls_ecdh_data_t {
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

typedef struct txp_ctx {
    coap_context_t* coap_ctx;
    coap_session_t* coap_session;
    cb_ctx_t cb_ctx;
    void* conn_data;
    uint8_t loop;
} txp_ctx;

#endif /* WITH_CONNECTION_LIBCOAP_RIOT */
#endif /* CONNECTION_LIBCOAP_H_ */
