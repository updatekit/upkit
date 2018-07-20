#ifndef FIREFLY_UTILS_H_
#define FIREFLY_UTILS_H_

// When using the cc2538 with Contiki, we need to reset the watchdog
// when performing long tasks, such as image verification.
// Since on the cc2538 there is no way to stop and restart the watchdog,
// we only reset it.
#ifdef CONTIKI
#include <dev/watchdog.h>
#define WATCHDOG_STOP() watchdog_periodic()
#define WATCHDOG_START() watchdog_periodic()
#else /* CONTIKI */
#define WATCHDOG_STOP()
#define WATCHDOG_START()
#endif /* CONTIKI */

#define INTERRUPTS_DISABLE() (void) irq_disable();
#define INTERRUPTS_ENABLE() (void) irq_enable();

#endif /* FIREFLY_UTILS_H_ */
