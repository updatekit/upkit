#include <libpull/common.h>
#include <libpull/memory.h>

#define WITH_BOOTLOADER
#include "platform_headers.h"

#define RESET_VECTOR 0x4

typedef void (*load_addr_t)(void);

void load_object(mem_id_t id) {
    mem_object_t obj;
    manifest_t mt;
    pull_error err;

    // Get image offset
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

    uint32_t destination_address = get_start_offset(id)+get_offset(&mt);

    INTERRUPTS_DISABLE();

    sys_clock_disable();

    __set_MSP(destination_address);
    __set_PSP(destination_address);

    load_addr_t load_addr = (load_addr_t)(*(uint32_t *)(destination_address + RESET_VECTOR));
    load_addr();
}
