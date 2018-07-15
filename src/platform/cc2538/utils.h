#ifndef FIREFLY_UTILS_H_
#define FIREFLY_UTILS_H_

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
