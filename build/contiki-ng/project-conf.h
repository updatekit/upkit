#ifndef _PROJECT_CONF_H
#define _PROJECT_CONF_H

// Define the RF Core channel
#undef RF_CHANNEL
#define RF_CHANNEL 25

#if EVALUATE_ENERGY == 1
#define ENERGEST_CONF_ON 1
#endif

#endif // _PROJECT_CONF_H
