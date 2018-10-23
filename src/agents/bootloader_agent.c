#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/security.h>

#include <libpull_agents/bootloader_agent.h>

#include "platform_headers.h"

static pull_error err;
static bootloader_ctx bctx;

static version_t version_bootable;
static version_t version_non_bootable;

static mem_object_t bootloader_object;
static mem_object_t source_slot;
static mem_object_t newest_bootable;
static mem_object_t swap_slot;
static mem_object_t obj_t;

static mem_id_t id_newest_non_bootable;
static mem_id_t id_newest_bootable;
static mem_id_t i = 0;

static address_t destination_address;
static manifest_t mt;

static bool factory = true;

#define BUFFER_SIZE 1024
uint8_t buffer[BUFFER_SIZE];

static uint8_t retry = 3;


agent_event_t bootloader_agent(bootloader_agent_config* cfg, void** event_data) {
    PULL_BEGIN(EVENT_INIT);

begin:
    PULL_CONTINUE(EVENT_GET_NEWEST_FIRMWARE, NULL);
    err = get_newest_firmware(&id_newest_bootable, &version_bootable, &obj_t, true);
    if (err) goto error;

    err = memory_open(&newest_bootable, id_newest_bootable, SEQUENTIAL_REWRITE);
    if (err) goto error;

    // (2) Bootstrap 
    PULL_CONTINUE(EVENT_BOOTSTRAP, NULL);
    err = memory_open(&bootloader_object, cfg->bootloader_ctx_id, WRITE_CHUNK);
    if (err) goto error;

    if (memory_read(&bootloader_object, &bctx, sizeof(bctx), 0x0) != sizeof(bctx)) {
        err = MEMORY_READ_ERROR;
        goto error;
    }

    if ((bctx.startup_flags & FIRST_BOOT) == FIRST_BOOT) {
        PULL_CONTINUE(EVENT_FIRST_BOOT, NULL);
        while (memory_slots[i].id != OBJ_END) {
            if (factory && memory_slots[i].reserved) {
                // (2.2) Store factory image
                PULL_CONTINUE(EVENT_STORE_FACTORY, NULL);
                log_debug("Storing Factory Image\n");
                err = memory_open(&source_slot, cfg->factory_id, WRITE_ALL);
                if (err) {
                    goto error;
                }
                PULL_CONTINUE(EVENT_STORE_FACTORY_COPY_START, NULL);
                err = copy_firmware(&newest_bootable, &source_slot, buffer, BUFFER_SIZE);
                memory_close(&source_slot);
                PULL_CONTINUE(EVENT_STORE_FACTORY_COPY_STOP, NULL);
                if (err) {
                    goto error;
                }
            } else if (memory_slots[i].id != id_newest_bootable) {
                err = invalidate_object(memory_slots[i].id, &obj_t);
                if (err) {
                    goto error;
                }
            }
            i++;
        }
        // (2.3) Store bootloader context
        PULL_CONTINUE(EVENT_STORE_BOOTLAODER_CTX, NULL);
        bctx.startup_flags = 0x0;
        if (memory_write(&bootloader_object, &bctx, sizeof(bctx), 0x0) != sizeof(bctx)) {
            err = MEMORY_WRITE_ERROR;
            goto error;
        };

        log_debug("Bootstrap Success\n");

    }
    memory_close(&bootloader_object);

    // (3) Get the newest non bootable firmware
    PULL_CONTINUE(EVENT_GET_NEWEST_NON_BOOTABLE, NULL);
    err = get_newest_firmware(&id_newest_non_bootable, &version_non_bootable, &obj_t, false);
    if (err) goto error;
    log_debug("id: bootable %d - non bootable %d\n", id_newest_bootable, id_newest_non_bootable);
    log_debug("version: bootable %x - non bootable %x\n", version_bootable, version_non_bootable);

    // (4) Compare them and upgrade if necessary
    if (version_bootable < version_non_bootable) {
        // (4.2) If valid, move it to the newest bootable slot
        PULL_CONTINUE(EVENT_UPGRADE, NULL);
        err = memory_open(&source_slot, id_newest_non_bootable, SEQUENTIAL_REWRITE);
        memory_close(&source_slot);
        if (err) goto error;

        PULL_CONTINUE(EVENT_UPGRADE_COPY_START, NULL);
        if (factory && memory_slots[id_newest_non_bootable].reserved) {
            err = copy_firmware(&source_slot, &newest_bootable, buffer, BUFFER_SIZE);
        } else {
            err = memory_open(&swap_slot, cfg->swap_id, SEQUENTIAL_REWRITE);
            if (err) goto error;
            err = swap_slots(&source_slot, &newest_bootable, &swap_slot, cfg->swap_size, buffer, BUFFER_SIZE);
            memory_close(&swap_slot);
        }

        PULL_CONTINUE(EVENT_UPGRADE_COPY_STOP, NULL);
        memory_close(&source_slot);
        if (err) goto error;

        PULL_CONTINUE(EVENT_UPGRADE_SUCCESS, NULL);
    }

    // (5) Validate the newest_bootable
    PULL_CONTINUE(EVENT_VALIDATE_BOOTABLE_START, NULL);
    do {
        err = verify_object(&newest_bootable, cfg->vendor_x, cfg->vendor_y, cfg->buffer, cfg->buffer_size);
    } while(err && retry--);
    PULL_CONTINUE(EVENT_VALIDATE_BOOTABLE_STOP, NULL);

    if (err) {
        // Invalidate the current image and restart from the top
        err = invalidate_object(id_newest_bootable, &obj_t);
        if (err) {
            goto error;
        }
        goto begin;
    }

    err = read_firmware_manifest(&newest_bootable, &mt);
    memory_close(&newest_bootable);
    if (err) goto error;

    destination_address = get_start_offset(id_newest_bootable)+get_offset(&mt);
    PULL_RETURN(EVENT_BOOT, &destination_address);
    PULL_FINISH(EVENT_FINISH);
error:
    *event_data = &err;
    return EVENT_FATAL_FAILURE;
}
