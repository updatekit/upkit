#include <libpull/common.h>
#include <libpull/memory.h>

#include "memory_cc2650.h"

#include "contiki.h"
#include "contiki-lib.h"
#include "ti-lib.h"
#include "dev/watchdog.h"

#define OTA_RESET_VECTOR    0x4

// Defined in Makefile.conf
#define IMAGE_START_OFFSET IMAGE_START_PAGE*PAGE_SIZE

void load_object(mem_id_t id) {
    mem_object_t obj;
    manifest_t mt;
    pull_error err;

    err = memory_open(&obj, id, READ_ONLY);
    if (err) {
        log_error(err, "Error opening the memory slot, aborting boot\n");
        return;
    }
    err = read_firmware_manifest(&obj, &mt);
    if (err) {
        log_error(err, "Error opening the memory slot, aborting boot\n");
        return;
    }
    memory_close(&obj);


    uint32_t destination_address = IMAGE_START_OFFSET+get_offset(&mt);
    log_debug("loading address %lx\n", destination_address);

    ti_lib_int_master_disable();
    HWREG(NVIC_VTABLE) = destination_address;

    destination_address += OTA_RESET_VECTOR;

    __asm("LDR R0, [%[dest]]"::[dest]"r"(destination_address));
    __asm("ORR R0, #1");
    __asm("BX R0");
}
