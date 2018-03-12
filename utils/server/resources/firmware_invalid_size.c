#include <stdio.h>
#include <coap/coap.h>
#include <coap/coap_dtls.h>

#include "memory/manifest.h"

#include "../server.h"

#define VERSION_MAX_AGE 3600

// How many blocks should the file be shorter?
#define SHORTER 10

// Considering that the client is using the size stored in the
// metadata to perform the request to the server, sending a size
// bigger than the actual file size will generate an invalid request
// because it will ask for a non present block
extern void resource_firmware_invalid_size(coap_context_t *ctx,
        struct coap_resource_t *resource, coap_session_t *session,
        coap_pdu_t *request, str *token, str *query, coap_pdu_t *response) {
    server_ctx_t* server_ctx = (server_ctx_t*) coap_get_app_data(ctx);
    // This is a blockwise transfer
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
    int block_size = block.szx << 4;

    int res = coap_write_block_opt(&block, COAP_OPTION_BLOCK2,
            response, server_ctx->mapped_file_len-(SHORTER*block_size));
    if (res != 1) {
        printf("Error writing block option\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }

    res = coap_add_block(response, server_ctx->mapped_file_len,
            server_ctx->mapped_file-(SHORTER*block_size), block.num, block.szx);
    if (res != 1) {
        printf("Error adding block data\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
}
