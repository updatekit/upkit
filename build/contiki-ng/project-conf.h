#ifndef _PROJECT_CONF_H
#define _PROJECT_CONF_H

// Define the RF Core channel
#undef RF_CHANNEL
#define RF_CHANNEL 25

#if LOGGER_VERBOSITY == 0
#define LOG_CONF_LEVEL_MAIN LOG_LEVEL_NONE
#endif

#endif // _PROJECT_CONF_H
