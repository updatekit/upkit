#ifndef PLATFORM_HEADERS_H_
#define PLATFORM_HEADERS_H_

#include "utils.h"

#if WITH_UPDATE_AGENT
#ifdef CONTIKI
#include <libpull_network/coap/connection_ercoap.h>
#else /* CONTIKI */
#include <libpull_network/coap/connection_libcoap_riot.h>
#endif /* CONTIKI */
#endif /* WITH_UPDATE_AGENT */

#endif /* PLATFORM_HEADERS_H_ */
