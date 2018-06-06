#ifndef SERVER_CONFIG_H_
#define SERVER_CONFIG_H_

#ifndef COAP_SERVER_PORT
#define COAP_SERVER_PORT 5683
#endif

#ifndef COAPS_SERVER_PORT
#define COAPS_SERVER_PORT 5684
#endif

#define MAX_ADDR_LEN 32
#define MAX_PATH_LEN 128

#ifndef WITH_TESTING_RESOURCES
#define WITH_TESTING_RESOURCES 1
#endif

#define DEFAULT_CURVE SECP256R1

#include "../../default_keys.h"

#endif /* SERVER_CONFIG_H_ */
