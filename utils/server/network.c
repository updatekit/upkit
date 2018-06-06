#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <coap/coap.h>
#include <coap/coap_dtls.h>

#include "config.h"

static int verify_key(const coap_session_t *session,
        const uint8_t *other_pub_x,
        const uint8_t *other_pub_y, 
        size_t key_size) {
    int validate = 0;
    validate |= memcmp(other_pub_x, dtls_client_x_g, key_size);
    validate |= memcmp(other_pub_y, dtls_client_y_g, key_size);
    printf("Client certificate validation: ");
    printf("%s\n", validate == 0? "success": "failure");
    return validate == 0? 1: -1;
}

coap_context_t* get_coap_context(const char *node) {
    coap_context_t* ctx = NULL;
    struct addrinfo hints;
    struct addrinfo* result,* rp;
    if (!coap_dtls_is_supported()) {
        fprintf(stderr, "DTLS is required but is not support by the library\n");
        return NULL;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* CoAP and CoAPS use UDP */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;

    int ret = getaddrinfo(node, NULL, &hints, &result);
    if ( ret != 0 ) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return NULL;
    }
    ctx = coap_new_context(NULL);
    if (!ctx) {
        freeaddrinfo(result);
        return NULL;
    }
    // Register PSK param
    coap_context_set_psk(ctx, psk_key.identity, psk_key.key, psk_key.len);
    // Register ECDSA param
    coap_context_set_ecdsa(ctx, DEFAULT_CURVE,
            ecdsa_priv_key, ecdsa_pub_key_x, ecdsa_pub_key_y, verify_key);

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        coap_address_t addr;
        coap_endpoint_t* ep = NULL;
        coap_endpoint_t* ep_dtls = NULL;
        if (rp->ai_addrlen <= sizeof(addr.addr)) {
            coap_address_init(&addr);
            addr.size = rp->ai_addrlen;
            memcpy(&addr.addr, rp->ai_addr, rp->ai_addrlen);
            // Bind UDP
            if (addr.addr.sa.sa_family == AF_INET) {
                addr.addr.sin.sin_port = htons(COAP_SERVER_PORT);
            } else if (addr.addr.sa.sa_family == AF_INET6) {
                addr.addr.sin6.sin6_port = htons(COAP_SERVER_PORT);
            } else {
                goto error;
            }
            printf("Binding on interface '%s' and port %d\n", node, COAP_SERVER_PORT);
            ep = coap_new_endpoint(ctx, &addr, COAP_PROTO_UDP);
            // Bind DTLS
            if (addr.addr.sa.sa_family == AF_INET) {
                addr.addr.sin.sin_port = htons(COAPS_SERVER_PORT);
            } else if (addr.addr.sa.sa_family == AF_INET6) {
                addr.addr.sin6.sin6_port = htons(COAPS_SERVER_PORT);
            } else {
                goto error;
            }
            printf("Binding on interface '%s' and port %d\n", node, COAPS_SERVER_PORT);
            ep_dtls = coap_new_endpoint(ctx, &addr, COAP_PROTO_DTLS);
            if (ep && ep_dtls)
                goto finish;
        }
    }
error:
    fprintf(stderr, "no context available for interface '%s'\n", node);
    coap_free_context(ctx);
finish:
    freeaddrinfo(result);
    return ctx;
}
