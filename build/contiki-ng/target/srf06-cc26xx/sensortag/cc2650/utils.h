#ifndef CC2650_UTILS_H_
#define CC2650_UTILS_H_

void flash_write_protect();

#define WATCHDOG_STOP() watchdog_stop()
#define WATCHDOG_START() watchdog_start()

#endif /* CC2650_UTILS_H_ */
