#include <coap/coap.h>
#include <coap/coap_dtls.h>
#include <stdio.h>

#include <libpull/network/receiver.h>
#include <libpull/memory/manifest.h>
#include <libpull/security/digest.h>
#include "../server.h"

extern void resource_next_version(coap_context_t *ctx,
        struct coap_resource_t *resource, coap_session_t *session,
        coap_pdu_t *request, str *token, str *query, coap_pdu_t *response) {
    server_ctx_t* server_ctx = (server_ctx_t*) coap_get_app_data(ctx);
    response->code = COAP_RESPONSE_CODE(205);

    uint8_t signature_buffer[64];
    // Increase version 
    version_t current_version = get_version(server_ctx->mapped_file_manifest);
    current_version++;
    set_version(server_ctx->mapped_file_manifest, current_version);

    /**************** Generate Vendor Signature ****************/
    pull_error err = sign_manifest_vendor(server_ctx->mapped_file_manifest,
            vendor_priv_g, (uint8_t*)&signature_buffer);
    if (err) {
        printf("Error signing Vendor data\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }

    printf("Version increased to %04x\n", current_version);
    coap_add_data(response, sizeof(version_t), (const uint8_t*) &current_version);
    return;
}
