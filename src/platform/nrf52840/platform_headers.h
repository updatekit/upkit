#ifndef PLATFORM_HEADERS_H_
#define PLATFORM_HEADERS_H_

#ifdef WITH_PLATFORM_NRF52840

#include "utils.h"

#ifdef WITH_BOOTLOADER
#include <arch/arm/cortex_m/cmsis.h>
#include <drivers/system_timer.h>
#endif

#ifndef DISABLE_LIBPULL_NETWORK
#include <libpull_network/coap/connection_zoap.h>
#endif

#endif /* WITH_PLATFORM_NRF52840 */
#endif /* PLATFORM_HEADERS_H_ */
