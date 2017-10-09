#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "common/error.h"
#include "common/callback.h"
#include "common/settings.h"

#ifndef _TRANSPORT_H
#define _TRANSPORT_H

/* The transport interface assumes you are using a rest protocol,
 * like HTTP or CoAP and that the generic methods are implemented
 */

typedef enum rest_method {
    /* Standard methods, valid for CoAP and HTTP*/
    GET,
    PUT,
    POST,
    DELETE,
    OPTIONS,
    /* CoAP specific methods */
    GET_BLOCKWISE2
} rest_method;

typedef enum conn_type conn_type;
typedef struct txp_ctx_ txp_ctx;

// conn_data can be null
pull_error txp_init(txp_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* data);

pull_error txp_on_data(txp_ctx* ctx, callback handler, void* more);

pull_error txp_request(txp_ctx* ctx, rest_method method, const char* resource, const char* data, uint16_t length);

pull_error txp_observe(txp_ctx* ctx, const char* resource, const char* token, uint8_t token_length);

void txp_end(txp_ctx* ctx);

#endif
