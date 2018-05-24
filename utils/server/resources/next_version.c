#include <coap/coap.h>
#include <coap/coap_dtls.h>
#include <stdio.h>

#include "../server.h"
#include "network/receiver.h"
#include "memory/manifest.h"
#include "security/sha256.h"

/* The vendor private key MUST never been in the server.
 * However, this is a testing server that requires to
 * generate and sign a new firmware at request. For this
 * reason the vendor key is included here */
static const uint8_t vendor_priv_g[32] = {
    0xf7,0xfd,0xc9,0x84,0x7a,0x1f,0xb2,0xac,0xed,0xc4,0x22,0x72,0x3b,0x88,0x5d,0x35,
    0xca,0x62,0x8c,0x51,0xce,0x41,0xe4,0x38,0xf0,0x64,0xe2,0xce,0xa5,0x9d,0xef,0x49
};

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
            tinycrypt_digest_sha256, vendor_priv_g, (uint8_t*)&signature_buffer,
            tinycrypt_secp256r1_ecc);
    if (err) {
        printf("Error signing Vendor data\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
    /**************** Invalidate Identity ****************/
    identity_t identity = { .udid = 0x0000, .random=0x0000 };
    set_identity(server_ctx->mapped_file_manifest, identity);
    printf("Version increased to %04x\n", current_version);
    coap_add_data(response, sizeof(version_t), (const uint8_t*) &current_version);
    return;
}
