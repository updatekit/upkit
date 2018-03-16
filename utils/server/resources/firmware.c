#include <stdio.h>
#include <coap/coap.h>
#include <coap/coap_dtls.h>

#include "../server.h"
#include "network/receiver.h"
#include "memory/manifest.h"
#include "security/sha256.h"

#define VERSION_MAX_AGE 3600

/* TODO read tihs from memory */
static const uint8_t priv_g[32] = {
    0xf7,0xfd,0xc9,0x84,0x7a,0x1f,0xb2,0xac,0xed,0xc4,0x22,0x72,0x3b,0x88,0x5d,0x35,
    0xca,0x62,0x8c,0x51,0xce,0x41,0xe4,0x38,0xf0,0x64,0xe2,0xce,0xa5,0x9d,0xef,0x49
};

DIGEST_FUNC(tinycrypt);

extern void resource_firmware(coap_context_t *ctx,
        struct coap_resource_t *resource, coap_session_t *session,
        coap_pdu_t *request, str *token, str *query, coap_pdu_t *response) {
    server_ctx_t* server_ctx = (server_ctx_t*) coap_get_app_data(ctx);

    receiver_msg_t* msg;
    size_t size = sizeof(receiver_msg_t);
    int err = coap_get_data(request, &size, (uint8_t**) &msg);
    if (err == 0) {
        printf("Received data is not correct\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }

    // Make the manifest specific for the request
    // TODO When moving the server to c++ store temporarely
    // the specific manifest for a N number of requuests
    // XXX now the signature is calculated at each request
    set_udid(server_ctx->mapped_file_manifest, msg->udid);
    set_random(server_ctx->mapped_file_manifest, msg->random);
    uint8_t signature_buffer[64];
    pull_error perr = sign_manifest_server(server_ctx->mapped_file_manifest, tinycrypt_digest_sha256, priv_g,
            (uint8_t*)&signature_buffer, tinycrypt_secp256r1_ecc);
    if (perr) {
        printf("Error signing data\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
            
    // Start the blockwise transfer
    unsigned char buf[3];
    response->code = COAP_RESPONSE_CODE(205);
    coap_add_option(response,
            COAP_OPTION_CONTENT_TYPE,
            coap_encode_var_bytes(buf, COAP_MEDIATYPE_APPLICATION_OCTET_STREAM), buf);

    coap_block_t block = { .num = 0, .m = 0, .szx = 4};
    coap_opt_iterator_t opt_iter;
    coap_opt_t* block_opt = coap_check_option(request, COAP_OPTION_BLOCK2, &opt_iter);
    if (block_opt) {
        block.num = coap_opt_block_num(block_opt);
        block.szx = COAP_OPT_BLOCK_SZX(block_opt);
    }
     int res = coap_write_block_opt(&block, COAP_OPTION_BLOCK2,
                          response,
                          server_ctx->mapped_file_len);
    if (res != 1) {
        printf("Error writing block option\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
    res = coap_add_block(response, server_ctx->mapped_file_len, server_ctx->mapped_file, block.num, block.szx);
    if (res != 1) {
        printf("Error adding block data\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
}
