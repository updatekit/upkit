#ifndef CONNECTION_H_
#define CONNECTION_H_

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

// To support blockwise requests the callback should be aware of the
// parameters used to perform the request, to create a new response with
// the correct data

#ifndef MAX_COAP_PAYLOAD_LEN
#define MAX_COAP_PAYLOAD_LEN 256
#endif

typedef struct txp_ctx {
    struct net_context *net_ctx;
    callback cb;
    void* more;
    struct sockaddr_in6 dest_addr;
    pull_error err;
    uint8_t loop;
} txp_ctx;

/* This callback is called everytime data are received */
void udp_receive(struct net_context *context,
    struct net_pkt *pkt, int status, void *user_data);


#endif /* CONNECTION_H_ */
