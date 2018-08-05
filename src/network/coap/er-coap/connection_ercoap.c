#ifdef WITH_CONNECTION_ERCOAP

#include <libpull/common.h>
#include <libpull/network.h>

#include "platform_headers.h"

#include "pt.h"

static conn_ctx* shared_ctx = NULL;

pull_error conn_init(conn_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* data) {
    // TODO validate input
    if (!coap_endpoint_parse(addr, strlen(addr), &ctx->endpoint)) {
        log_error(CONNECTION_INIT_ERROR, "Error parsing the endpoint\n");
        return CONNECTION_INIT_ERROR;
    }
    if (!coap_endpoint_connect(&ctx->endpoint)) {
        log_error(CONNECTION_INIT_ERROR, "Error connecting to the endpoint\n");
        return CONNECTION_INIT_ERROR;
    }

    return PULL_SUCCESS;
}

pull_error conn_on_data(conn_ctx* ctx, callback handler, void* more) {
    if (handler == NULL) {
        log_error(CALLBACK_ERROR, "Error, received callback was null\n");
        return CALLBACK_ERROR;
    }
    ctx->cb = handler;
    ctx->more = more;
    return PULL_SUCCESS;
}

pull_error conn_request(conn_ctx* ctx, rest_method method, const char* resource, const char* data, uint16_t length) {
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

void break_loop(conn_ctx* ctx) {
    ctx->request_state.response = NULL;
    process_post(ctx->request_state.process, PROCESS_EVENT_POLL, NULL);
}

void ercoap_handler(coap_message_t* response) {
    const unsigned char* chunk;
    int len = coap_get_payload(response, &chunk);
    shared_ctx->cb(PULL_SUCCESS, (const char*) chunk, len, shared_ctx->more);
}

pull_error conn_observe(conn_ctx* ctx, const char* resource, const char* token, uint8_t token_length) {
    return PULL_SUCCESS;
}

inline void conn_end(conn_ctx* ctx) {
    // TODO this must be implemented and it is important for DTLS because in
    // case of error is useful to close the connection and free the resources
}

#endif /* WITH_CONNECTION_ERCOAP */
