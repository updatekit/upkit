#ifdef WITH_CONNECTION_LIBCOAP

#include <libpull_network/coap/connection_libcoap.h> 

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <coap/coap.h>
#include <coap/block.h>

#include "platform_headers.h"

#ifdef SYSTEM_RESOLVER
#include <netdb.h>
#endif

#define CALLBACK(a,b,c) ((void(*)())((cb_ctx_t*)ctx->app)->cb)(a,b,c,((cb_ctx_t*)ctx->app)->more)
#define BCTX ((cb_ctx_t*)ctx->app)->bctx

#define BLOCK_SIZE 4
#define URI_SPLIT_SIZE 128

static pull_error split_resource(const char* resource, coap_pdu_t* pdu);

static uint8_t method_mapper[] = {
    [GET] = COAP_REQUEST_GET,
    [PUT] = COAP_REQUEST_PUT,
    [POST] = COAP_REQUEST_POST,
    [DELETE] = COAP_REQUEST_DELETE,
    [GET_BLOCKWISE2] = COAP_REQUEST_GET
};

static int libcoap_verify_key(const coap_session_t *session, const uint8_t *other_pub_x,
        const uint8_t *other_pub_y, size_t key_size) {
    // TODO Get the conn_ctx and call the verify key
    return 1;
}

pull_error conn_init(conn_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* conn_data) {
    ctx->coap_ctx = coap_new_context(NULL);
    if (ctx->coap_ctx == NULL) {
        log_error(CONNECTION_INIT_ERROR, "Failure creating the CoAP context\n");
        return CONNECTION_INIT_ERROR;
    }
    ctx->conn_data = conn_data;
    // Configure the connection
    coap_proto_t proto;
    uint16_t final_port;
    switch (type) {
        case PULL_UDP:
            proto = COAP_PROTO_UDP;
            final_port = !port? COAP_DEFAULT_PORT: port;
            break;
#ifndef DISABLE_DTLS
        case PULL_DTLS_PSK:
            proto = COAP_PROTO_DTLS;
            final_port = !port? COAPS_DEFAULT_PORT: port;
            dtls_psk_data_t* psk_data = (dtls_psk_data_t*) conn_data;
            if (!psk_data) {
                return INVALID_CONN_DATA_ERROR;
            }
            coap_context_set_psk(ctx->coap_ctx, psk_data->hint, 
                    psk_data->key, psk_data->key_len);
            break;
        case PULL_DTLS_ECDH:
            proto = COAP_PROTO_DTLS;
            final_port = !port? COAPS_DEFAULT_PORT: port;
            dtls_ecdh_data_t* ecdh_data = (dtls_ecdh_data_t*) conn_data;
            if (!ecdh_data) {
                return INVALID_CONN_DATA_ERROR;
            }
            coap_context_set_ecdsa(ctx->coap_ctx, SECP256R1,
                    ecdh_data->priv_key, ecdh_data->pub_key_x,
                    ecdh_data->pub_key_y, libcoap_verify_key);
            break;
#endif
        default:
            log_error(INVALID_CONN_DATA_ERROR, "Protocol not supported\n");
            return INVALID_CONN_DATA_ERROR;
    }
#ifdef DEBUG_LIBCOAP
    coap_set_log_level(LOG_DEBUG);
    coap_dtls_set_log_level(LOG_DEBUG);
#endif
#ifdef SYSTEM_RESOLVER
    // Resolve the address
    struct addrinfo hints;
    struct addrinfo *result = NULL, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_ALL;
    int error = getaddrinfo(addr, NULL, &hints, &result);
    if (error) {
        log_error(RESOLVER_ERROR, "getaddrinfo: %s\n", gai_strerror(error));
        return RESOLVER_ERROR;
    }
    // Create a libcoap session
    ctx->coap_session = NULL;
    log_debug("Resolving host %s and port %d\n", addr, final_port);
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        coap_address_t bind_addr;
        if (rp->ai_addrlen <= sizeof(bind_addr.addr) && (rp->ai_family == AF_INET6 || rp->ai_family == AF_INET)) {
            coap_address_init(&bind_addr);
            bind_addr.size = rp->ai_addrlen;
            memcpy(&bind_addr.addr, rp->ai_addr, rp->ai_addrlen);
            bind_addr.addr.sin.sin_port = htons(final_port);
            ctx->coap_session = coap_new_client_session(ctx->coap_ctx, NULL, &bind_addr, proto);
            if (ctx->coap_session)
                break;
        }
    }
    freeaddrinfo(result);
#endif /* SYSTEM_RESOLVER */
#ifdef RIOT_RESOLVER
    ipv6_addr_t ipv6_addr;
    ipv6_addr_from_str(&ipv6_addr, addr);

    coap_address_t bind_addr;
    coap_address_init(&bind_addr);
    memcpy(&bind_addr.addr.sin6.sin6_addr, &ipv6_addr, sizeof(ipv6_addr));
    bind_addr.size = sizeof(struct sockaddr_in6);
    bind_addr.addr.sin6.sin6_port = htons(final_port);
    bind_addr.addr.sa.sa_family = AF_INET6;

    ctx->coap_session = coap_new_client_session(ctx->coap_ctx, NULL, &bind_addr, proto);
#endif /* RIOT_RESOLVER */
    return ctx->coap_session == NULL ? RESOLVER_ERROR : PULL_SUCCESS;
}

void handler_libcoap(struct coap_context_t * ctx,
        coap_session_t *session, coap_pdu_t *sent,
        coap_pdu_t *received, const coap_tid_t id) {
    unsigned char buf[4];
    if (received->type == COAP_MESSAGE_RST) {
        log_error(REQUEST_RST_ERROR, "Received a reset message\n");
        CALLBACK(REQUEST_RST_ERROR, NULL, 0);
        return;
    }
    // Check if we received an error
    if (COAP_RESPONSE_CLASS(received->code) >= 4) {
        // TODO Here I should parse the errors and return the
        // correct one to the callback
        log_error(REQUEST_ERROR, "Received an error %d.%02d\n",
                (received->code >> 5), received->code & 0x1F);
        CALLBACK(REQUEST_ERROR, NULL, 0);
        return;
    }
    // If we are here then the request was successfull
    // Check if the block options is set
    coap_opt_iterator_t opt_iter;
    coap_opt_t* block_opt = coap_check_option(received, COAP_OPTION_BLOCK2, &opt_iter);
    if (block_opt && BCTX.enabled) {
        /// TODO I should check the Etag option to see if they are correct
        if(COAP_OPT_BLOCK_MORE(block_opt)) {
            uint16_t blktype = opt_iter.type;
            coap_pdu_t* pdu = coap_pdu_init(COAP_MESSAGE_CON, BCTX.method,
                    coap_new_message_id(session), coap_session_max_pdu_size(session));
            if (!pdu) {
                log_error(REQUEST_ERROR, "Error creating the request\n");
                CALLBACK(REQUEST_ERROR, NULL, 0);
                return;
            }
            if (split_resource(BCTX.resource, pdu) != PULL_SUCCESS) {
                log_error(REQUEST_ERROR, "Error splitting resource\n");
                coap_delete_pdu(pdu);
                CALLBACK(REQUEST_ERROR, NULL, 0);
                return;
            }
            coap_add_option(pdu, blktype, coap_encode_var_bytes(buf,
                        ((coap_opt_block_num(block_opt) + 1) << 4) |
                        BLOCK_SIZE), buf);
            if (BCTX.length > 0 && BCTX.data != NULL) {
                if (coap_add_data(pdu, BCTX.length, (const uint8_t*) BCTX.data) == 0) {
                    log_error(REQUEST_ERROR, "Error adding data\n");
                    coap_delete_pdu(pdu);
                    CALLBACK(REQUEST_ERROR, NULL, 0);
                    return;
                }
            }
            if (coap_send(session, pdu) == COAP_INVALID_TID) {
                log_error(REQUEST_ERROR, "Error sending the PDU\n");
                CALLBACK(REQUEST_ERROR, NULL, 0);
                return;
            }
        }
    } else if (block_opt && !BCTX.enabled) {
        // The response contained a blockwise transmission but it was
        // not expected by the client. Aborting
        CALLBACK(REQUEST_ERROR, NULL, 0);
        return;
    }
    // Get the data and call the callback
    unsigned char* data;
    size_t len = -1;
    int ret = coap_get_data(received, &len, &data);
    if (ret != 1) {
        CALLBACK(REQUEST_ERROR, NULL, 0);
        return;
    }
    CALLBACK(PULL_SUCCESS, data, len);
    //coap_delete_pdu(received);
}

// Consider that in this way you can have just one handler for
// each context! You should be sure to set just one handler for
// all the received packets. If you need to manage more connections
// simultaneously you should use two context and two sessions!
pull_error conn_on_data(conn_ctx* ctx, callback handler, void* more) {
    if (ctx == NULL || handler == NULL) {
        return GENERIC_ERROR; // TODO SPECIALIZE THIS ERROR
    }
    ctx->cb_ctx.cb = handler;
    ctx->cb_ctx.more = more;
    ctx->coap_ctx->app = &ctx->cb_ctx;
    coap_register_response_handler(ctx->coap_ctx, handler_libcoap);
    return PULL_SUCCESS;
}

static pull_error split_resource(const char* resource, coap_pdu_t* request) {
    size_t size = URI_SPLIT_SIZE;
    unsigned char buffer[URI_SPLIT_SIZE];
    unsigned char* buffer_p = buffer;
    int res = coap_split_path((const unsigned char*) resource, strlen(resource), buffer, &size);
    while (res--) {
        coap_add_option(request, COAP_OPTION_URI_PATH,
                coap_opt_length(buffer_p), coap_opt_value(buffer_p));
        buffer_p += coap_opt_size(buffer_p);
    }
    return PULL_SUCCESS;
}

pull_error conn_request(conn_ctx* ctx, rest_method method, const char* resource, 
        const char* data, uint16_t length) {
    coap_pdu_t* request = coap_pdu_init(COAP_MESSAGE_CON, method_mapper[method],
            coap_new_message_id(ctx->coap_session), 
            coap_session_max_pdu_size(ctx->coap_session));
    if (!request) {
        log_error(REQUEST_ERROR, "Error creating the request\n");
        return REQUEST_ERROR;
    }
    if (split_resource(resource, request) != PULL_SUCCESS) {
        log_error(INVALID_RESOURCE_ERROR, "Error splitting resource\n");
        coap_delete_pdu(request);
        return INVALID_RESOURCE_ERROR;
    }
    if (method ==  GET_BLOCKWISE2) {
        ctx->cb_ctx.bctx.enabled = 1;
        ctx->cb_ctx.bctx.method = method_mapper[method];
        ctx->cb_ctx.bctx.resource = resource;
        ctx->cb_ctx.bctx.data = data;
        ctx->cb_ctx.bctx.length = length;
        uint8_t buf[4];
        // Start always from the block 0
        coap_add_option(request, COAP_OPTION_BLOCK2, 
                coap_encode_var_bytes(buf, 0 << 4 | BLOCK_SIZE), buf);
    } else {
        ctx->cb_ctx.bctx.enabled = 0;
    }
    if (length > 0 && data != NULL) {
        ctx->cb_ctx.bctx.data = data;
        ctx->cb_ctx.bctx.length = length;
        if (coap_add_data(request, length, (const uint8_t*) data) == 0) {
            log_error(INVALID_DATA_ERROR, "Error adding data\n");
            coap_delete_pdu(request);
            return INVALID_DATA_ERROR;
        }
    }
    if (coap_send(ctx->coap_session, request) == COAP_INVALID_TID) {
        log_error(SEND_ERROR, "Error sending the PDU\n");
        coap_delete_pdu(request);
        return SEND_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error conn_observe(conn_ctx* ctx, const char* resource, const char* token, uint8_t token_length) {
    return PULL_SUCCESS;
}

void conn_end(conn_ctx* ctx) {
    if (ctx->coap_session != NULL) {
        coap_session_release(ctx->coap_session);
    }
    if (ctx->coap_ctx != NULL) {
        coap_free_context(ctx->coap_ctx);
    }
    coap_cleanup();
}

#endif /* WITH_CONNECTION_LIBCOAP */
