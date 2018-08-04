#ifndef UTILS_H_
#define UTILS_H_

#define WATCHDOG_STOP() watchdog_stop()
#define WATCHDOG_START() watchdog_stop()

#define INTERRUPTS_DISABLE()
#define INTERRUPTS_ENABLE()

#define SET_VTOR(addr) HWREG(NVIC_VTABLE) = addr

#endif /* FIREFLY_UTILS_H_ */
