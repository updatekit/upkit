#ifndef CC2538_PLATFORM_HEADERS_H_
#define CC2538_PLATFORM_HEADERS_H_

#include "utils.h"
#include "cpu.h"

#if WITH_UPDATE_AGENT
#ifdef CONTIKI
#include <libpull_network/coap/connection_ercoap.h>
#else /* CONTIKI */
#include <libpull_network/coap/connection_libcoap_riot.h>
#endif /* CONTIKI */
#endif /* WITH_UPDATE_AGENT */

#endif /* CC2538_PLATFORM_HEADERS_H_ */
