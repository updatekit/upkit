#include <coap/coap.h>
#include <coap/coap_dtls.h>

#include "../server.h"

#define VERSION_MAX_AGE 3600

extern void resource_version_invalid(coap_context_t *ctx,
        struct coap_resource_t *resource, coap_session_t *session,
        coap_pdu_t *request, str *token, str *query, coap_pdu_t *response) {
    unsigned char buf[3];
    response->code = COAP_RESPONSE_CODE(205);

    coap_add_option(response,
            COAP_OPTION_CONTENT_TYPE,
            coap_encode_var_bytes(buf, COAP_MEDIATYPE_APPLICATION_OCTET_STREAM), buf);
    // Max age rapresents the number of seconds the data could be cached
    coap_add_option(response,
            COAP_OPTION_MAXAGE,
            coap_encode_var_bytes(buf, VERSION_MAX_AGE), buf);
    uint16_t invalid_version = 0x0;
    coap_add_data(response, sizeof(uint16_t), (const unsigned char*) &invalid_version);
}
