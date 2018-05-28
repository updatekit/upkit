#include <libpull/common.h>
#include <libpull/network.h>

#include "connection_ercoap.h"

#include "pt.h"

static txp_ctx* shared_ctx = NULL;

pull_error txp_init(txp_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* data) {
    // TODO validate input
    if (!coap_endpoint_parse(addr, strlen(addr), &ctx->endpoint)) {
        log_error(TRANSPORT_INIT_ERROR, "Error parsing the endpoint\n");
        return TRANSPORT_INIT_ERROR;
    }
    if (!coap_endpoint_connect(&ctx->endpoint)) {
        log_error(TRANSPORT_INIT_ERROR, "Error connecting to the endpoint\n");
        return TRANSPORT_INIT_ERROR;
    }

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

void ercoap_handler(coap_message_t* response) {
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
