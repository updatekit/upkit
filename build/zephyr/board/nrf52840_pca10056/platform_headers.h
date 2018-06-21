#ifndef PLATFORM_HEADERS_H_
#define PLATFORM_HEADERS_H_

#include "memory_nrf52840_pca10056.h"
#include "loader.h"
#include "utils.h"

#ifdef BOOTLOADER
#include <arch/arm/cortex_m/cmsis.h>
#include <drivers/system_timer.h>
#endif

#ifndef DISABLE_LIBPULL_NETWORK
#include "connection.h"
#endif

#endif /* PLATFORM_HEADERS_H_ */
