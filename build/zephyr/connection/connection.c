#include "connection.h"

#define BLOCK_SIZE 4
#define URI_SPLIT_SIZE 128

static uint8_t method_mapper[] = {
    [GET] = COAP_METHOD_GET,
    [PUT] = COAP_METHOD_PUT,
    [POST] = COAP_METHOD_POST,
    [DELETE] = COAP_METHOD_DELETE,
    [GET_BLOCKWISE2] = COAP_METHOD_GET
};

#define CALLBACK(a,b,c) 

pull_error txp_init(txp_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* conn_data) {
    int res;

	if (net_addr_pton(AF_INET6, addr, &ctx->dest_addr.sin6_addr)) {
        log_error(TRANSPORT_INIT_ERROR, "Invalid address\n");
		return TRANSPORT_INIT_ERROR;
	}
    ctx->dest_addr.sin6_family = AF_INET6;
    ctx->dest_addr.sin6_port = htons(COAP_DEFAULT_PORT);

    res = net_context_get(PF_INET6, SOCK_DGRAM, IPPROTO_UDP, &ctx->net_ctx);
    if (res != 0) {
        log_error(TRANSPORT_INIT_ERROR, "Error getting an UDP context\n");
        return TRANSPORT_INIT_ERROR;
    }
    

    struct sockaddr_in6 my_addr;
    if (net_addr_pton(AF_INET6, CONFIG_NET_APP_MY_IPV6_ADDR,
                &my_addr.sin6_addr)) {
        NET_ERR("Invalid my IPv6 address: %s",
                CONFIG_NET_APP_MY_IPV6_ADDR);
        return -1;
    }
    my_addr.sin6_family = AF_INET6;

    res = net_context_bind(ctx->net_ctx, (struct sockaddr *) &my_addr,
            sizeof(my_addr));
    if (res) {
        NET_ERR("Could not bind the context");
        return TRANSPORT_INIT_ERROR;
    }

    // This allows to obtain the context from the callback
    ctx->net_ctx->user_data = ctx;
    return PULL_SUCCESS;
}

void udp_receive(struct net_context *context,
        struct net_pkt *pkt, int status, void *user_data) {
    struct txp_ctx* ctx = (txp_ctx*)user_data;
    struct coap_packet response;

    if (status < 0) {
        log_debug("Error receiving data\n");
        ctx->cb(REQUEST_ERROR, NULL, 0, ctx->more);
        return;
    }

    if (coap_packet_parse(&response, pkt, NULL, 0) != 0) {
        log_debug("Error, invalid coap message received\n");
        ctx->cb(REQUEST_ERROR, NULL, 0, ctx->more);
        return;
    }

    // TODO If it is a blockwise request continue

    struct net_buf *frag;
    uint16_t offset;
    uint16_t len;
    frag = coap_packet_get_payload(&response, &offset, &len);
    if (!frag && offset == 0xffff) {
        log_error(REQUEST_ERROR, "Error getting the packet payload\n");
        ctx->cb(REQUEST_ERROR, NULL, 0, ctx->more);
        return;
    }
    if (len == 0) {
        ctx->cb(PULL_SUCCESS, NULL, 0, ctx->more);
        return;
    }

    if (len > MAX_COAP_PAYLOAD_LEN) {
        log_error(REQUEST_ERROR, "Received payload bigger than payload buffer\n");
        ctx->cb(REQUEST_ERROR, NULL, 0, ctx->more);
        return;
    }
    const char payload[MAX_COAP_PAYLOAD_LEN];
    frag = net_frag_read(frag, offset, &offset, len, (u8_t *) payload);
	if (!frag && offset == 0xffff) {
        log_error(REQUEST_ERROR, "Error getting the packet payload\n");
        ctx->cb(REQUEST_ERROR, NULL, 0, ctx->more);
        return;
	}

    ctx->cb(PULL_SUCCESS, &payload, len, ctx->more);
}

pull_error txp_on_data(txp_ctx* ctx, callback handler, void* more) {
    ctx->cb = handler;
    ctx->more = more;
    return PULL_SUCCESS;
}

pull_error txp_request(txp_ctx* ctx, rest_method method, const char* resource, 
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
    if (pkt == NULL) {
        log_error(REQUEST_ERROR, "Not enough memory\n");
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
        return REQUEST_ERROR;
    }

    // Split resource
    // Resource will be somethings like "/one[/two[/three]]"
    // Thus we can split it simply finding the slash char
    const char *a, *p = resource;
    for (a = p+1; a <= (resource+strlen(resource)); a++) {
        if (*p != '/') {
            log_error(REQUEST_ERROR, "The URI is not valid!\n");
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

pull_error txp_observe(txp_ctx* ctx, const char* resource, const char* token, uint8_t token_length) {
    /* NOT IMPLEMENTED */
    return PULL_SUCCESS;
}

void txp_end(txp_ctx* ctx) {
    if (net_context_put(ctx->net_ctx) < 0) {
        log_warn("Error closing the transport\n");
    }
    ctx->net_ctx = NULL;
}
