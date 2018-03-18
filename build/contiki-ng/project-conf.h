#ifndef _PROJECT_CONF_H
#define _PROJECT_CONF_H

// Define the RF Core channel
#undef RF_CHANNEL
#define RF_CHANNEL 25

// Reduce the number of connections
#define UIP_CONF_UDP_CONNS 4

#define LOG_CONF_LEVEL_MAIN LOG_LEVEL_NONE

#endif // _PROJECT_CONF_H
