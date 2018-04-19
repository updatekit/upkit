#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

// Reduce the size of the network modules
#define NETSTACK_CONF_FRAMER  framer_nullmac
#define NETSTACK_CONF_LLSEC   nullsec_driver
#define NETSTACK_CONF_MAC     nullmac_driver
#define NETSTACK_CONF_RDC     nullrdc_driver

// Reduce size removing process names
#define PROCESS_CONF_NO_PROCESS_NAMES 1

#define UIP_CONF_TCP 0
#define UIP_CONF_UDP 0

#if LOGGER_VERBOSITY == 0
#define LOG_CONF_LEVEL_MAIN LOG_LEVEL_NONE
#endif

#if EVALUATE_ENERGY == 1
#define ENERGEST_CONF_ON 1
#endif

#endif /* PROJECT_CONF_H_ */
