#include <coap/coap.h>
#include <coap/coap_dtls.h>

extern void resource_echo(coap_context_t *ctx,
        struct coap_resource_t *resource, coap_session_t *session,
        coap_pdu_t *request, str *token, str *query, coap_pdu_t *response) {
    size_t size;
    unsigned char *data;
    coap_get_data(request, &size, &data);
    if (size == 0) {
        // No data received. No echo possible. Sending a 400 Bad Request
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
    response->code = COAP_RESPONSE_CODE(205);
    coap_opt_iterator_t opt_iter;
    coap_opt_t* content_type = coap_check_option(request, COAP_OPTION_CONTENT_TYPE, &opt_iter);
    if (content_type) {
        coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                coap_opt_length(content_type), coap_opt_value(content_type));
    }
    coap_add_data(response, size, data);
}
