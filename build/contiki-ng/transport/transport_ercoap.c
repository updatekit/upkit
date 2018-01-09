#include "network/transport.h"
#include "common/error.h"
#include "common/logger.h"
#include "transport_ercoap.h"

#include "pt.h"

#if defined(CONN_DTLS_PSK) || defined(CONN_DTLS_ECDSA)
#include "coap-dtls.h"
#endif

static txp_ctx* shared_ctx = NULL;

// The received string will always be ignored by this implementation
pull_error txp_init(txp_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* data) {
    switch (type) {
#ifdef CONN_UDP
        case TXP_UDP:
            ctx->port = COAP_DEFAULT_PORT;
            break;
#elif CONN_DTLS_PSK
        case TXP_DTLS_PSK:
            ctx->port = COAPS_DEFAULT_PORT;
            coap_set_psk_data((dtls_psk_data_t*) data);
            break;
#elif CONN_DTLS_ECDSA
        case TXP_DTLS_ECDSA:
            ctx->port = COAPS_DEFAULT_PORT;
            coap_set_ecdsa_data((dtls_ecdsa_data_t*) data);
            break;
#endif

        default:
            return TRANSPORT_INIT_ERROR;
    }
    coap_init_engine();
    HARDCODED_PROV_SERVER();
    return PULL_SUCCESS;
}

pull_error txp_on_data(txp_ctx* ctx, callback handler, void* more) {
    if (handler == NULL) {
        log_error(CALLBACK_ERROR, "Error, received callback was null\n");
        return CALLBACK_ERROR;
    }
    ctx->cb = handler;
    ctx->more = more;
    return PULL_SUCCESS;
}

pull_error txp_request(txp_ctx* ctx, rest_method method, const char* resource, const char* data, uint16_t length) {
    shared_ctx = ctx;
    coap_init_message(&ctx->request, COAP_TYPE_CON, COAP_GET, 0);
    log_debug("Sending request of lenght %d to resource %s\n", length, resource);
    coap_set_header_uri_path(&ctx->request, resource);
    if (data != NULL && length != 0) {
        coap_set_header_block2(&ctx->request, 0, 0, 64);
        coap_set_payload(&ctx->request, (uint8_t *) data, length);
    }
    return PULL_SUCCESS;
}

void break_loop(txp_ctx* ctx) {
    ctx->request_state.response = NULL;
    process_post(ctx->request_state.process, PROCESS_EVENT_POLL, NULL);
}

void ercoap_handler(void* response) {
    const unsigned char* chunk;
    int len = coap_get_payload(response, &chunk);
    shared_ctx->cb(PULL_SUCCESS, (const char*) chunk, len, shared_ctx->more);
}

pull_error txp_observe(txp_ctx* ctx, const char* resource, const char* token, uint8_t token_length) {
    return PULL_SUCCESS;
}

inline void txp_end(txp_ctx* ctx) {
    // TODO this must be implemented and it is important for DTLS because in
    // case of error is useful to close the connection and free the resources
}
