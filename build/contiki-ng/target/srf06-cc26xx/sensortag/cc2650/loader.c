#include "common/libpull.h"
#include "memory/manifest.h"
#include "memory/memory.h"

#define OTA_RESET_VECTOR    0x4

#include "contiki.h"
#include "contiki-lib.h"
#include "ti-lib.h"
#include "dev/watchdog.h"

void load_object(obj_id id, manifest_t* mt) {
    uint32_t destination_address = 0;
    /// XXX HARDCODED VALUE
    destination_address = 0x5000+get_offset(mt);
    log_info("loading address %lx\n", destination_address);

    ti_lib_int_master_disable();
    HWREG(NVIC_VTABLE) = destination_address;

    destination_address += OTA_RESET_VECTOR;

    __asm("LDR R0, [%[dest]]"::[dest]"r"(destination_address));
    __asm("ORR R0, #1");
    __asm("BX R0");
}
