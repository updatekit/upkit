#ifndef CONNECTION_H_
#define CONNECTION_H_

#ifdef WITH_CONNECTION_ZOAP

#include <libpull/common.h>
#include <libpull/network/connection_interface.h>
#include <libpull/security/ecc.h>

#include <zephyr.h>
#include <net/net_pkt.h>
#include <net/net_mgmt.h>
#include <net/net_ip.h>
#include <net/udp.h>
#include <net/coap.h>

#define COAP_DEFAULT_PORT 5683
#define COAPS_DEFAULT_PORT 5684

typedef struct {
    uint8_t method;
    uint8_t enabled;
    const char* resource;
    const char* data;
    uint16_t length;
} blockwise_request_t;

#ifndef MAX_COAP_PAYLOAD_LEN
#define MAX_COAP_PAYLOAD_LEN 256
#endif

typedef struct {
    callback cb;
    void* more;
    blockwise_request_t bctx;
    struct coap_block_context block;
} cb_ctx_t;

typedef struct txp_ctx {
    struct net_context *net_ctx;
    struct sockaddr_in6 dest_addr;
    pull_error err;
    cb_ctx_t cb_ctx;
    void* conn_data;
    uint8_t loop;
} txp_ctx;

/* This callback is called everytime data are received */
void udp_receive(struct net_context *context,
    struct net_pkt *pkt, int status, void *user_data);

#endif /* WITH_CONNECTION_ZOAP */
#endif /* CONNECTION_H_ */
