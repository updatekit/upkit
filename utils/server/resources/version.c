#include <coap/coap.h>
#include <coap/coap_dtls.h>
#include <stdio.h>

#include "../server.h"

#define VERSION_MAX_AGE 3600

extern void resource_version(coap_context_t *ctx,
        struct coap_resource_t *resource, coap_session_t *session,
        coap_pdu_t *request, str *token, str *query, coap_pdu_t *response) {
    server_ctx_t* server_ctx = (server_ctx_t*) coap_get_app_data(ctx);
    unsigned char buf[3];
    response->code = COAP_RESPONSE_CODE(205);

    if (coap_find_observer(resource, session, token)) {
        coap_add_option(response,
                COAP_OPTION_OBSERVE,
                coap_encode_var_bytes(buf, resource->observe), buf);
    }

    coap_add_option(response,
            COAP_OPTION_CONTENT_TYPE,
            coap_encode_var_bytes(buf, COAP_MEDIATYPE_APPLICATION_OCTET_STREAM), buf);
    // Max age rapresents the number of seconds the data could be cached
    coap_add_option(response,
            COAP_OPTION_MAXAGE,
            coap_encode_var_bytes(buf, VERSION_MAX_AGE), buf);
    version_t version = get_version(&server_ctx->mt);
    printf("Sending version %x\n", version);
    coap_add_data(response, sizeof(version_t), (const uint8_t*) &version);
}
