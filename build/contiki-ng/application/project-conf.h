#ifndef _PROJECT_CONF_H
#define _PROJECT_CONF_H

// Define the RF Core channel
#define IEEE802154_CONF_DEFAULT_CHANNEL 26

#define COAP_DTLS_PSK_DEFAULT_IDENTITY "ThisIsMyIdentity"
#define COAP_DTLS_PSK_DEFAULT_KEY      "ThisIsOurSecret1"

#define HARDCODED_PROV_SERVER(addr) uip_ip6addr(addr,0xfd00,0x0,0x0,0x0,0x0,0x0,0x0,0x1)

#endif // _PROJECT_CONF_H
