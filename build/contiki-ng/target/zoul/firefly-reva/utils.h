#ifndef FIREFLY_UTILS_H_
#define FIREFLY_UTILS_H_

#include <dev/watchdog.h>

// Considering that the cc2538 does not have a way to block the watchdog, we
// only reset the timer.

#define WATCHDOG_STOP() watchdog_periodic()
#define WATCHDOG_START() watchdog_periodic()

#endif /* CC2650_UTILS_H_ */
