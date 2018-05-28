#include <stdio.h>
#include <coap/coap.h>
#include <coap/coap_dtls.h>

#include "../server.h"
#include <libpull/network.h>
#include <libpull/memory/manifest.h>
#include <libpull/security.h>

#define VERSION_MAX_AGE 3600

/* TODO read tihs from memory */
static const uint8_t priv_g[32] = {
    0xf7,0x60,0x33,0x57,0xde,0x87,0x5e,0x0b,0x34,0x1f,0xb2,0x9e,0x60,0x07,0xcc,0x7c,
    0xea,0x51,0x16,0x35,0xe9,0x2e,0x7c,0x8d,0xaf,0x0e,0x10,0xee,0x12,0xc0,0x10,0xfb
};

extern void resource_firmware(coap_context_t *ctx,
        struct coap_resource_t *resource, coap_session_t *session,
        coap_pdu_t *request, str *token, str *query, coap_pdu_t *response) {
    server_ctx_t* server_ctx = (server_ctx_t*) coap_get_app_data(ctx);

    receiver_msg_t* msg;
    uint8_t* raw_message;
    size_t size = sizeof(receiver_msg_t);
    int err = coap_get_data(request, &size, &raw_message);
    if (err == 0) {
        printf("Received data is not correct\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
    if (size != sizeof(receiver_msg_t)) {
        printf("Received data size is not valid\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
    msg = (receiver_msg_t*) raw_message;

    // Make the manifest specific for the request
    // TODO When moving the server to c++ store temporarely
    // the specific manifest for a N number of requuests
    // XXX now the signature is calculated at each request
    // TODO This part must be completely rewritten to make it more robust
    identity_t current_identity = get_identity(server_ctx->mapped_file_manifest);
    identity_t identity = {
        .udid = msg->udid,
        .random = msg->random
    };
    if (current_identity.udid != identity.udid || current_identity.random != identity.random) {
        set_identity(server_ctx->mapped_file_manifest, identity);
        uint8_t signature_buffer[64];
        pull_error perr = sign_manifest_server(server_ctx->mapped_file_manifest, 
                tinycrypt_digest_sha256, priv_g, (uint8_t*)&signature_buffer, tinycrypt_secp256r1_ecc);
        if (perr) {
            printf("Error signing data\n");
            response->code = COAP_RESPONSE_CODE(400);
            return;
        }
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
    // Set the block size requested by the client
    if (block_opt) {
        block.num = coap_opt_block_num(block_opt);
        block.szx = COAP_OPT_BLOCK_SZX(block_opt);
    }
    if (msg->offset >= server_ctx->mapped_file_len) {
        response->code = COAP_RESPONSE_CODE(404);
        printf("Requested invalid size\n");
        return;
    }

    uint8_t* file_pointer = server_ctx->mapped_file + msg->offset;
    size_t remaining_len = server_ctx->mapped_file_len - msg->offset;
    int res = coap_write_block_opt(&block, COAP_OPTION_BLOCK2, response, remaining_len);
    if (res != 1) {
        printf("Error writing block option\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
    res = coap_add_block(response, remaining_len, file_pointer, block.num, block.szx);
    if (res != 1) {
        printf("Error adding block data\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
}
