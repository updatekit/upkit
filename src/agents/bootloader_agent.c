#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/security.h>

#include <libpull_agents/bootloader_agent.h>

#include "platform_headers.h"

static pull_error err;
static mem_object_t obj_t;

static bootloader_ctx bctx;

static version_t version_bootable;
static version_t version_non_bootable;

static mem_object_t bootloader_object;
static mem_object_t source_slot;
static mem_object_t destination_slot;
static mem_object_t newest_bootable;
static mem_object_t swap_slot;

static mem_id_t id_newest_non_bootable;
static mem_id_t id_newest_bootable;
static mem_id_t id_oldest_bootable;
static mem_id_t boot_id;
static mem_id_t i;

#define BUFFER_SIZE 1024

uint8_t buffer[BUFFER_SIZE];

static bool already_validated_flag = false;

pull_error restore_factory_image() {
    return GENERIC_ERROR;
}

agent_event_t bootloader_agent(bootloader_agent_config* cfg, void** event_data) {
    PULL_BEGIN(EVENT_INIT);

    // (1) Get the newest bootable firmware (there must be at least one!!)
    PULL_CONTINUE(EVENT_GET_NEWEST_FIRMWARE, NULL);
    err = get_newest_firmware(&id_newest_bootable, &version_bootable, &obj_t, true);
    if (err) {
        PULL_RETURN(EVENT_GET_NEWEST_FIRMWARE_FAILURE, &err);
    }
    err = memory_open(&newest_bootable, id_newest_bootable, READ_ONLY);
    if (err) {
        PULL_RETURN(EVENT_GET_NEWEST_FIRMWARE_FAILURE_2, &err);
    }

    // (2) Bootstrap 
    PULL_CONTINUE(EVENT_BOOTSTRAP, NULL);
    err = memory_open(&bootloader_object, cfg->bootloader_ctx_id, WRITE_CHUNK);
    if (err) {
        PULL_RETURN(EVENT_BOOTSTRAP_FAILURE, &err);
    }
    if (memory_read(&bootloader_object, &bctx, sizeof(bctx), 0x0) != sizeof(bctx)) {
        err = MEMORY_READ_ERROR;
        PULL_RETURN(EVENT_BOOTSTRAP_FAILURE_2, &err);
    }
    if ((bctx.startup_flags & FIRST_BOOT) == FIRST_BOOT) {
        // (2.1) Erase slots
        PULL_CONTINUE(EVENT_FIRST_BOOT, NULL);
        for (i = 0; memory_slots[i].id != OBJ_END; i++) {
            if (memory_slots[i].id != id_newest_bootable) {
                err = invalidate_object(memory_slots[i].id, &obj_t);
                if (err) {
                    PULL_RETURN(EVENT_FIRST_BOOT_FAILURE, &err);
                }
            }
        }
#if FACTORY_IMAGE
        // (2.2) Store factory image
        PULL_CONTINUE(EVENT_STORE_FACTORY, NULL);
        log_debug("Storing Factory Image\n");
        err = memory_open(&destination_slot, cfg->factory_id, WRITE_ALL);
        if (err) {
            PULL_RETURN(EVENT_STORE_RECOVER_FAILURE, &err);
        }
        PULL_CONTINUE(EVENT_STORE_FACTORY_COPY_START, NULL)
        err = copy_firmware(&newest_bootable, &destination_slot, buffer, BUFFER_SIZE);
        PULL_CONTINUE(EVENT_STORE_FACTORY_COPY_STOP, NULL)
        if (err) {
            PULL_RETURN(EVENT_STORE_FACTORY_COPY_FAILURE, &err);
        }
#endif /* FACTORY_IMAGE == 1 */
        // (2.3) Store bootloader context
        PULL_CONTINUE(EVENT_STORE_BOOTLAODER_CTX, NULL);
        bctx.startup_flags = 0x0;
        if (memory_write(&bootloader_object, &bctx, sizeof(bctx), 0x0) != sizeof(bctx)) {
            err = MEMORY_WRITE_ERROR;
            PULL_RETURN(EVENT_STORE_BOOTLAODER_CTX_FAILURE, &err);
        };
        log_debug("Bootstrap Success\n");
    }
    memory_close(&bootloader_object);

    // (3) Get the newest non bootable firmware
    PULL_CONTINUE(EVENT_GET_NEWEST_NON_BOOTABLE, NULL);
    err = get_newest_firmware(&id_newest_non_bootable, &version_non_bootable, &obj_t, false);
    if (err) {
        PULL_RETURN(EVENT_GET_NEWEST_NON_BOOTABLE_FAILURE, &err);
    }
    log_debug("id: bootable %d - non bootable %d\n", id_newest_bootable, id_newest_non_bootable);
    log_debug("version: bootable %x - non bootable %x\n", version_bootable, version_non_bootable);

    // (4) Compare them and upgrade if necessary
    if (version_bootable < version_non_bootable) {
        // (4.1) Validate the newest non bootable slot
        PULL_CONTINUE(EVENT_VALIDATE_NON_BOOTABLE, NULL);
        err = memory_open(&source_slot, id_newest_non_bootable, READ_ONLY);
        if (err) {
           PULL_RETURN(EVENT_VALIDATE_NON_BOOTABLE_FAILURE, &err);
           // XXX This error can be recovered
        }
        PULL_CONTINUE(EVENT_VALIDATE_NON_BOOTABLE_START, NULL);
        err = verify_object(&source_slot, cfg->df, cfg->vendor_x, cfg->vendor_y, cfg->ef, 
                        cfg->buffer, cfg->buffer_size);
        PULL_CONTINUE(EVENT_VALIDATE_NON_BOOTABLE_STOP, NULL);
        if (err) {
           // XXX  invalidate_object(id, &obj_t);
            PULL_CONTINUE(EVENT_VALIDATE_NON_BOOTABLE_INVALID, &err);
           // XXX This error can be recovered
        }
        memory_close(&source_slot);

        // (4.2) If valid, move it to the oldest bootable slot
        PULL_CONTINUE(EVENT_UPGRADE, NULL);
        err = get_oldest_firmware(&id_oldest_bootable, &version_bootable, &obj_t, true);
        if (err) {
            PULL_RETURN(EVENT_UPGRADE_FAILURE, &err);
        }
        err = memory_open(&destination_slot, id_oldest_bootable, SEQUENTIAL_REWRITE);
        if (err) {
            PULL_RETURN(EVENT_UPGRADE_FAILURE_2, &err);
        }
        err = memory_open(&source_slot, id_newest_non_bootable, SEQUENTIAL_REWRITE);
        if (err) {
            PULL_RETURN(EVENT_UPGRADE_FAILURE_3, &err);

        }
        err = memory_open(&swap_slot, cfg->swap_id, SEQUENTIAL_REWRITE);
        if (err) {
            PULL_RETURN(EVENT_UPGRADE_FAILURE_4, &err);
        }

        PULL_CONTINUE(EVENT_UPGRADE_COPY_START, NULL);
        err = swap_slots(&source_slot, &destination_slot, &swap_slot, cfg->swap_size, buffer, BUFFER_SIZE);
        //err = copy_firmware(&source_slot, &destination_slot, buffer, BUFFER_SIZE);
        PULL_CONTINUE(EVENT_UPGRADE_COPY_STOP, NULL);
        if (err) {
            PULL_RETURN(EVENT_UPGRADE_COPY_FAILURE, &err);
        }
        memory_close(&destination_slot);
        PULL_CONTINUE(EVENT_UPGRADE_SUCCESS, NULL);
        already_validated_flag = true;
        boot_id = id_oldest_bootable;
    }
    // (5) Check if the validation has been already performed during the upgrade
    if (!already_validated_flag) {
        // (5.1) Validate the newest_bootable
        PULL_CONTINUE(EVENT_VALIDATE_BOOTABLE_START, NULL);
        err = verify_object(&newest_bootable, cfg->df, cfg->vendor_x, cfg->vendor_y, cfg->ef, cfg->buffer, cfg->buffer_size);
        PULL_CONTINUE(EVENT_VALIDATE_BOOTABLE_STOP, NULL);
        if (!err) {
            boot_id = id_newest_bootable;
        } else {
            PULL_RETURN(EVENT_VALIDATE_BOOTABLE_FAILURE, &err);
#if FACTORY_IMAGE
            // (5.1.1) Restore the factory image if available
            PULL_CONTINUE(EVENT_FACTORY_RESET, NULL);
            PULL_CONTINUE(EVENT_FACTORY_RESET_START, NULL);
            err = restore_factory_image(&id_newest_bootable);
            PULL_CONTINUE(EVENT_FACTORY_RESET_STOP, NULL);
            if (!err) {
                boot_id = id_newest_bootable;
            } else {
                PULL_RETURN(EVENT_FATAL_FAILURE, NULL);
            }
#else 
            PULL_RETURN(EVENT_FATAL_FAILURE, NULL);
#endif
        }
    }
    PULL_RETURN(EVENT_BOOT, &boot_id);
    PULL_FINISH(EVENT_FINISH);
}
