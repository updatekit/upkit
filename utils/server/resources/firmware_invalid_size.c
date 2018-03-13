#include <stdio.h>
#include <coap/coap.h>
#include <coap/coap_dtls.h>

#include "memory/manifest.h"
#include "../server.h"

manifest_t invalid_manifest = {
    .vendor.size = 0xffffffff,
    .vendor.version = 0xffff,
    .vendor.platform = 0xffff,
};

#define VERSION_MAX_AGE 3600
/** Considering that the client is using the size stored in the
  metadata to perform the request to the server, sending a size
  bigger than the actual file size will generate an invalid request
  because it will ask for a non present block */
extern void resource_firmware_invalid_size(coap_context_t *ctx,
        struct coap_resource_t *resource, coap_session_t *session,
        coap_pdu_t *request, str *token, str *query, coap_pdu_t *response) {
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
    int res = coap_write_block_opt(&block, COAP_OPTION_BLOCK2,
            response,
            sizeof(manifest_t));
    if (res != 1) {
        printf("Error writing block option\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
    res = coap_add_block(response, sizeof(manifest_t), (uint8_t*) &invalid_manifest, block.num, block.szx);
    if (res != 1) {
        printf("Error adding block data\n");
        response->code = COAP_RESPONSE_CODE(400);
        return;
    }
}
