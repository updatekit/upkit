#ifndef RESOURCES_H_
#define RESOURCES_H_

#include <coap/coap.h>
#include <coap/coap_dtls.h>

#include "../server.h"
    
#define DEFINE_RESOURCE(r, resource, title) { \
    r = coap_resource_init((unsigned char*) resource, strlen(resource), 0); \
    coap_add_attr(r, (unsigned char *)"ct", 2, (unsigned char *)"0", 1, 0); \
    coap_add_attr(r, (unsigned char *)"title", 5, (unsigned char *)#title, strlen(title)+2, 0); \
}

#define REGISTER_HANDLER(r, method, handler, is_observable) { \
    void handler(coap_context_t*, struct coap_resource_t*, \
            coap_session_t*, coap_pdu_t*, str*, str*, coap_pdu_t*); \
    coap_register_handler(r, method, handler); \
    r->observable = is_observable; \
}

#define REGISTER_RESOURCE(ctx, r) { \
    coap_add_resource(ctx, resource); \
}

int init_resources(server_ctx_t* server_ctx, coap_context_t* context) {
    coap_resource_t *resource;
    /* RESOURCE Echo */
    DEFINE_RESOURCE(resource, "echo", "Echo server");
    REGISTER_HANDLER(resource, COAP_REQUEST_GET, resource_echo, 0);
    REGISTER_RESOURCE(context, resource);
    /* RESOURCE Version */
    DEFINE_RESOURCE(resource, "version", "Get the latest version");
    REGISTER_HANDLER(resource, COAP_REQUEST_GET, resource_version, 1);
    REGISTER_RESOURCE(context, resource);
    server_ctx->resource_version = resource;
    /* RESOURCE Firmware */
    DEFINE_RESOURCE(resource, "firmware", "Get the latest firmware");
    REGISTER_HANDLER(resource, COAP_REQUEST_GET, resource_firmware, 0);
    REGISTER_RESOURCE(context, resource);
#if WITH_TESTING_RESOURCES
    /* RESOURCE Version Invalid */
    DEFINE_RESOURCE(resource, "version/invalid", "Get an invalid version");
    REGISTER_HANDLER(resource, COAP_REQUEST_GET, resource_version_invalid, 1);
    REGISTER_RESOURCE(context, resource);
    /* RESOURCE Firmware Invalid Size*/
    DEFINE_RESOURCE(resource, "firmware/invalid_size", "Get the latest firmware");
    REGISTER_HANDLER(resource, COAP_REQUEST_GET, resource_firmware_invalid_size, 0);
    REGISTER_RESOURCE(context, resource);
    /* RESOURCE Firmware Invalid Signature*/
    DEFINE_RESOURCE(resource, "firmware/invalid_signature", "Get the latest firmware");
    REGISTER_HANDLER(resource, COAP_REQUEST_GET, resource_firmware_invalid_signature, 0);
    REGISTER_RESOURCE(context, resource);
    /* RESOURCE Next Version */
    DEFINE_RESOURCE(resource, "next_version", "Update the firmware to the next version");
    REGISTER_HANDLER(resource, COAP_REQUEST_GET, resource_next_version, 0);
    REGISTER_RESOURCE(context, resource);
#endif //WITH_TESTING_RESOURCES
    return 1;
}
#undef RESOURCE
#endif
