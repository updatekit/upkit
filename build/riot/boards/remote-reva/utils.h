#ifndef FIREFLY_UTILS_H_
#define FIREFLY_UTILS_H_

// Considering that the cc2538 does not have a way to block the watchdog, we
// only reset the timer.

#define WATCHDOG_STOP()
#define WATCHDOG_START()
#define INTERRUPTS_DISABLE() (void) irq_disable();
#define INTERRUPTS_ENABLE() (void) irq_enable();

#endif /* FIREFLY_UTILS_H_ */
