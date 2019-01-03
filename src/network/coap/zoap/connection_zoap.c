#ifdef WITH_CONNECTION_ZOAP

#include "platform_headers.h"

static uint8_t method_mapper[] = {
    [GET] = COAP_METHOD_GET,
    [PUT] = COAP_METHOD_PUT,
    [POST] = COAP_METHOD_POST,
    [DELETE] = COAP_METHOD_DELETE,
    [GET_BLOCKWISE2] = COAP_METHOD_GET
};

#define CALLBACK(a,b,c) (ctx->cb_ctx.cb)(a,b,c,(ctx->cb_ctx.more))
#define BCTX ctx->cb_ctx.bctx

#define BLOCK_SIZE COAP_BLOCK_64

pull_error conn_init(conn_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* conn_data) {
    int res;

    ctx->conn_data = conn_data;
	if (net_addr_pton(AF_INET6, addr, &ctx->dest_addr.sin6_addr)) {
        log_error(CONNECTION_INIT_ERROR, "Invalid address\n");
		return CONNECTION_INIT_ERROR;
	}
    ctx->dest_addr.sin6_family = AF_INET6;
    ctx->dest_addr.sin6_port = htons(COAP_DEFAULT_PORT);

    res = net_context_get(PF_INET6, SOCK_DGRAM, IPPROTO_UDP, &ctx->net_ctx);
    if (res != 0) {
        log_error(CONNECTION_INIT_ERROR, "Error getting an UDP context\n");
        return CONNECTION_INIT_ERROR;
    }

    struct sockaddr_in6 my_addr;
    if (net_addr_pton(AF_INET6, addr,
                &my_addr.sin6_addr)) {
        log_error(CONNECTION_INIT_ERROR, "Invalid my IPv6 address: %s", addr);
        return CONNECTION_INIT_ERROR;
    }
    my_addr.sin6_family = AF_INET6;

    res = net_context_bind(ctx->net_ctx, (struct sockaddr *) &my_addr,
            sizeof(my_addr));
    if (res) {
        log_error(CONNECTION_INIT_ERROR, "Could not bind the context");
        return CONNECTION_INIT_ERROR;
    }

    // This allows to obtain the context from the callback
    ctx->net_ctx->user_data = ctx;
    return PULL_SUCCESS;
}

void udp_receive(struct net_context *context,
        struct net_pkt *pkt, int status, void *user_data) {
    struct conn_ctx* ctx = (conn_ctx*)user_data;
    struct coap_packet response;

    if (status < 0) {
        log_debug("Error receiving data\n");
        CALLBACK(REQUEST_ERROR, NULL, 0);
        goto end;
    }

    if (coap_packet_parse(&response, pkt, NULL, 0) != 0) {
        log_debug("Error, invalid coap message received\n");
        CALLBACK(REQUEST_ERROR, NULL, 0);
        goto end;
    }

    // TODO If it is a blockwise request continue
    struct coap_block_context current_block;
    if (coap_next_block(&response, &current_block) && BCTX.enabled) {
        // There are more blocks avaiable on the server!
        struct coap_packet request;
        struct net_pkt *rpkt;
        struct net_buf *frag;
        int r;

        rpkt = net_pkt_get_tx(ctx->net_ctx, K_FOREVER);
        if (rpkt == NULL) {
            log_error(REQUEST_ERROR, "Not enough memory\n");
            CALLBACK(REQUEST_ERROR, NULL, 0);
            return;
        }
        frag = net_pkt_get_data(ctx->net_ctx, K_FOREVER);
        if (frag == NULL) {
            log_error(REQUEST_ERROR, "Not enough memory\n");
            net_pkt_unref(rpkt);
            CALLBACK(REQUEST_ERROR, NULL, 0);
            return;
        }

        net_pkt_frag_add(rpkt, frag);

        r = coap_packet_init(&request, rpkt, 1, COAP_TYPE_CON,
                8, coap_next_token(),
                BCTX.method, coap_next_id());

        if (r < 0) {
            log_error(REQUEST_ERROR, "Error sending the request\n");
            net_pkt_unref(rpkt);
            CALLBACK(REQUEST_ERROR, NULL, 0);
            return;
        }

        if (strlen(BCTX.resource) < sizeof("/")) {
            log_error(REQUEST_ERROR, "Invalid resource\n");
            net_pkt_unref(rpkt);
            CALLBACK(REQUEST_ERROR, NULL, 0);
            return;
        }

        // Split resource
        // Resource will be somethings like "/one[/two[/three]]"
        // Thus we can split it simply finding the slash char
        const char *a, *p = BCTX.resource;
        for (a = p+1; a <= (BCTX.resource+strlen(BCTX.resource)); a++) {
            if (*p != '/') {
                log_error(REQUEST_ERROR, "The URI is not valid!\n");
                net_pkt_unref(rpkt);
                CALLBACK(REQUEST_ERROR, NULL, 0);
                return;
            }
            if (*a != '/' && *a != '\0') {
                continue;
            }
            r = coap_packet_append_option(&request, COAP_OPTION_URI_PATH,
                    p+1, a-p-1);
            if (r < 0) {
                log_error(REQUEST_ERROR, "Error splitting string\n");
                net_pkt_unref(rpkt);
                CALLBACK(REQUEST_ERROR, NULL, 0);
                return;
            }
            if (*a == '\0') {
                break;
            }
            p=a++;
        }

        r = coap_update_from_block(&response, &ctx->cb_ctx.block);
        if (r < 0 ) {
            log_warn("Error updating from block\n");
            net_pkt_unref(rpkt);
            CALLBACK(REQUEST_ERROR, NULL, 0);
            return;
        }
        r = !coap_next_block(&response, &ctx->cb_ctx.block);
        if (r < 0) {
            log_warn("Error appending block option 2\n");
            net_pkt_unref(rpkt);
            CALLBACK(REQUEST_ERROR, NULL, 0);
            return;
        }
        r = coap_append_block2_option(&request, &ctx->cb_ctx.block);
        if (r < 0 ) {
            log_warn("Error appending block option 3\n");
            net_pkt_unref(rpkt);
            CALLBACK(REQUEST_ERROR, NULL, 0);
            return;
        }

        // Add data
        if (BCTX.data != NULL && BCTX.length > 0) {
            r = coap_packet_append_payload_marker(&request);
            if (r < 0) {
                log_warn("Unable to append payload marker");
                net_pkt_unref(rpkt);
                CALLBACK(REQUEST_ERROR, NULL, 0);
                return;
            }
            r = coap_packet_append_payload(&request, (uint8_t *)BCTX.data, BCTX.length);
            if (r < 0) {
                log_warn("Not able to append payload");
                net_pkt_unref(rpkt);
                CALLBACK(REQUEST_ERROR, NULL, 0);
                return;
            }
        }
        r = net_context_sendto(rpkt, (struct sockaddr *) &ctx->dest_addr,
                sizeof(struct sockaddr_in6), NULL, K_NO_WAIT, NULL, NULL);

        if (r) {
            log_error(REQUEST_ERROR, "Error sending packet\n");
            net_pkt_unref(rpkt);
            CALLBACK(REQUEST_ERROR, NULL, 0);
            return;
        }
    }

    struct net_buf *frag;
    uint16_t offset;
    uint16_t len;
    frag = coap_packet_get_payload(&response, &offset, &len);
    if (!frag && offset == 0xffff) {
        log_error(REQUEST_ERROR, "Error getting the packet payload\n");
        CALLBACK(REQUEST_ERROR, NULL, 0);
        goto end;
    }
    if (len == 0) {
        CALLBACK(PULL_SUCCESS, NULL, 0);
        goto end;
    }

    if (len > MAX_COAP_PAYLOAD_LEN) {
        log_error(REQUEST_ERROR, "Received payload bigger than payload buffer\n");
        CALLBACK(REQUEST_ERROR, NULL, 0);
        goto end;
    }
    const char payload[MAX_COAP_PAYLOAD_LEN];
    frag = net_frag_read(frag, offset, &offset, len, (u8_t *) payload);
    if (!frag && offset == 0xffff) {
        log_error(REQUEST_ERROR, "Error getting the packet payload\n");
        CALLBACK(REQUEST_ERROR, NULL, 0);
        goto end;
    }
    // If it contains the blockwise option and is a blockwise request, continue

    CALLBACK(PULL_SUCCESS, (const char*) &payload, len);
end:
    net_pkt_unref(pkt);
}

pull_error conn_on_data(conn_ctx* ctx, callback handler, void* more) {
    if (ctx == NULL || handler == NULL) {
        return GENERIC_ERROR; // TODO SPECIALIZE THIS ERROR
    }
    ctx->cb_ctx.cb = handler;
    ctx->cb_ctx.more = more;
    return PULL_SUCCESS;
}

pull_error conn_request(conn_ctx* ctx, rest_method method, const char* resource, 
        const char* data, uint16_t length) {
    struct coap_packet request;
    struct net_pkt *pkt;
    struct net_buf *frag;
    int r;

    pkt = net_pkt_get_tx(ctx->net_ctx, K_FOREVER);
    if (pkt == NULL) {
        log_error(REQUEST_ERROR, "Not enough memory\n");
        return REQUEST_ERROR;
    }
    frag = net_pkt_get_data(ctx->net_ctx, K_FOREVER);
    if (frag == NULL) {
        log_error(REQUEST_ERROR, "Not enough memory\n");
        net_pkt_unref(pkt);
        return REQUEST_ERROR;
    }

    net_pkt_frag_add(pkt, frag);

    r = coap_packet_init(&request, pkt, 1, COAP_TYPE_CON,
            8, coap_next_token(),
            method_mapper[method], coap_next_id());

    if (r < 0) {
        log_error(REQUEST_ERROR, "Error sending the request\n");
        net_pkt_unref(pkt);
        return REQUEST_ERROR;
    }

    if (strlen(resource) < sizeof("/")) {
        log_error(REQUEST_ERROR, "Invalid resource\n");
        net_pkt_unref(pkt);
        return REQUEST_ERROR;
    }

    // Split resource
    // Resource will be somethings like "/one[/two[/three]]"
    // Thus we can split it simply finding the slash char
    const char *a, *p = resource;
    for (a = p+1; a <= (resource+strlen(resource)); a++) {
        if (*p != '/') {
            log_error(REQUEST_ERROR, "The URI is not valid!\n");
            net_pkt_unref(pkt);
            return REQUEST_ERROR;
        }
        if (*a != '/' && *a != '\0') {
            continue;
        }
        r = coap_packet_append_option(&request, COAP_OPTION_URI_PATH,
                p+1, a-p-1);
        if (r < 0) {
            log_error(REQUEST_ERROR, "Error splitting string\n");
            net_pkt_unref(pkt);
            return REQUEST_ERROR;
        }
        if (*a == '\0') {
            break;
        }
        p=a++;
    }

    if (method ==  GET_BLOCKWISE2) {
        ctx->cb_ctx.bctx.enabled = 1;
        ctx->cb_ctx.bctx.method = method_mapper[method];
        ctx->cb_ctx.bctx.resource = resource;
        ctx->cb_ctx.bctx.data = data;
        ctx->cb_ctx.bctx.length = length;

        r = coap_block_transfer_init(&ctx->cb_ctx.block, BLOCK_SIZE, 0);
        if (r < 0 ) {
            log_warn("Error creating block_context");
            net_pkt_unref(pkt);
            return REQUEST_ERROR;
        }

        r = coap_append_block2_option(&request, &ctx->cb_ctx.block);
        if (r < 0 ) {
            log_warn("Error appending block option");
            net_pkt_unref(pkt);
            return REQUEST_ERROR;
        }
    } else {
        ctx->cb_ctx.bctx.enabled = 0;
    }

    // Add data
    if (data != NULL && length > 0) {
        r = coap_packet_append_payload_marker(&request);
        if (r < 0) {
            log_warn("Unable to append payload marker");
            net_pkt_unref(pkt);
            return REQUEST_ERROR;
        }
        r = coap_packet_append_payload(&request, (uint8_t *)data, length);
        if (r < 0) {
            log_warn("Not able to append payload");
            net_pkt_unref(pkt);
            return REQUEST_ERROR;
        }
    }

    r = net_context_sendto(pkt, (struct sockaddr *) &ctx->dest_addr,
            sizeof(struct sockaddr_in6), NULL, K_NO_WAIT, NULL, NULL);
    if (r) {
        log_error(REQUEST_ERROR, "Error sending packet\n");
        net_pkt_unref(pkt);
        return REQUEST_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error conn_observe(conn_ctx* ctx, const char* resource, const char* token, uint8_t token_length) {
    /* NOT IMPLEMENTED */
    return PULL_SUCCESS;
}

void conn_end(conn_ctx* ctx) {
    if (net_context_put(ctx->net_ctx) < 0) {
        log_warn("Error closing the connection\n");
    }
    ctx->net_ctx = NULL;
}

#endif /* WITH_CONNECTION_ZOAP */
