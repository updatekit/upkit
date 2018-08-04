#ifdef WITH_ARM_CORTEX_LOADER

#include <libpull/common.h>
#include <libpull/memory.h>

#include "platform_headers.h"

#define RESET_VECTOR    0x4

typedef void (*load_addr_t)(void);

void load_object(mem_id_t id) {
    mem_object_t obj;
    manifest_t mt;
    pull_error err;

    register uint32_t msp __asm("msp");
    register uint32_t psp __asm("psp");

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
    log_debug("loading address %lx\n", destination_address);

    INTERRUPTS_DISABLE();

    // Update vector table address
    SET_VTOR(destination_address);

    // Update stack
    uint32_t stack = *(uint32_t *)(destination_address);
    msp = stack;
    psp = stack;

    // Jump to address
    load_addr_t load_addr = (load_addr_t)(*(uint32_t *)(destination_address + RESET_VECTOR));
    load_addr();
}
#endif /* WITH_ARM_CORTEX_LOADER */
