#ifndef UTILS_H_
#define UTILS_H_

#define WATCHDOG_STOP()
#define WATCHDOG_START()
#define INTERRUPTS_DISABLE() irq_lock()
#define INTERRUPTS_ENABLE()

#define SET_VTOR(addr) SCB->VTOR = addr

#endif /* UTILS_H_ */
