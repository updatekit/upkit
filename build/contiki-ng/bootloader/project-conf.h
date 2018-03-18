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

#define LOG_CONF_LEVEL_MAIN LOG_LEVEL_NONE

#endif /* PROJECT_CONF_H_ */
