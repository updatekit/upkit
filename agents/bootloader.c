#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/security.h>

static pull_error err;
static mem_object_t obj_t;

static mem_object_t bootloader_object;
static bootloader_ctx bctx;

static version_t version_bootable;
static version_t version_non_bootable;
static mem_object_t destination_slot;
static mem_object_t newest_bootable;

static mem_id_t id_newest_non_bootable;
static mem_id_t id_oldest_bootable;

#define BUFFER_SIZE 1024

uint8_t buffer[BUFFER_SIZE];

static bool already_validated_flag = false;

pull_error restore_recovery_image() {
    return GENERIC_ERROR;
}

agent_msg_t bootloader_agent(bootloader_agent_config* cfg) {
    PULL_BEGIN(EVENT_INIT);

    // (1) Get the newest bootable firmware (XXX there must be at least one!!)
    PULL_CONTINUE(EVENT_GET_NEWEST_FIRMWARE, NULL);
    err = get_newest_firmware(&id_newest_bootable, &version_bootable, &obj_t, true, true);
    if (err) {
        PULL_CONTINUE(EVENT_GET_NEWEST_FIRMWARE_FAILURE, &err);
    }
    err = memory_open(&newest_bootable, id_newest_bootable, READ_ALL);
    if (err) {
        PULL_CONTINUE(EVENT_GET_NEWEST_FIRMWARE_FAILURE_2, &err);
    }

    // (2) Bootstrap 
    PULL_CONTINUE(EVENT_BOOTSTRAP, NULL);
    err = memory_open(&bootloader_ctx_object, cfg->bootloader_ctx_id, WRITE_CHUNK);
    if (ctx->err) {
        PULL_CONTINUE(EVENT_BOOTSTRAP_FAILURE, &ctx->err);
    }
    if (memory_read(&bootloader_object, &bctx, sizeof(bctx), 0x0) != sizeof(bctx)) {
        err = MEMORY_READ_ERROR;
        PULL_CONTINUE(EVENT_BOOTSTRAP_FAILURE_2, &err);
    }
    if ((ctx.startup_flags & FIRST_BOOT) == FIRST_BOOT) {
        // (2.1) Erase slots
        PULL_CONTINUE(EVENT_FIRST_BOOT, NULL);
        mem_id_t i;
        for (i = 0; memory_slots[i].id != OBJ_END; i++) {
            if (!memory_slot[i].loaded) {
                err = invalidate_object(memory_slots[i], &obj_t);
                if (err) {
                    PULL_CONTINUE(EVENT_FIRST_BOOT_FAILURE, &err);
                }
            }
        }
#if RECOVERY_IMAGE == 1
        // (2.2) Store recovery image
        PULL_CONTINUE(EVENT_STORE_RECOVERY, NULL);
        log_debug("Storing Recovery Image\n");
        err = memory_open(&destination_slot, cfg->recover_id, WRITE_ALL);
        if (err) {
            PULL_CONTINUE(EVENT_STORE_RECOVER_FAILURE, &err);
        }
        PULL_CONTINUE(EVENT_STORE_RECOVERY_COPY_START, NULL)
        err = copy_firmware(&newest_bootable, &destination_slot, buffer, BUFFER_SIZE);
        PULL_CONTINUE(EVENT_STORE_RECOVERY_COPY_STOP, NULL)
        if (err) {
            PULL_CONTINUE(EVENT_STORE_RECOVERY_COPY_FAILURE, &err);
        }
#endif /* RECOVERY_IMAGE == 1 */
        // (2.3) Store bootloader context
        PULL_CONTINUE(EVENT_STORE_BOOTLAODER_CTX, NULL);
        ctx.startup_flags = 0x0;
        if (memory_write(&bootloader_object, &ctx, sizeof(ctx), 0x0) != sizeof(ctx)) {
            err = MEMORY_WRITE_ERROR;
            PULL_CONTINUE(EVENT_STORE_BOOTLAODER_CTX_FAILURE, &err);
        };
        log_debug("Bootstrap Success\n");
    }
    memory_close(&bootloader_object);

    // (3) Get the newest non bootable firmware
    PULL_CONTINUE(EVENT_GET_NEWEST_NON_BOOTABLE);
    err = get_newest_firmware(&id_newest_non_bootable, &version_non_bootable, &obj_t, true, false);
    if (err) {
        PULL_CONTINUE(EVENT_GET_NEWEST_NON_BOOTABLE_FAILURE, &err);
    }

    // (4) Compare them and upgrade if necessary
    if (version_bootable < version_non_bootable) {

        // (4.1) Validate the newest non bootable slot
        PULL_CONTINUE(EVENT_VALIDATE_NON_BOOTABLE, NULL);
        err = memory_open(&obj_t, &from, READ_ONLY);
        if (err) {
           PULL_CONTINUE(EVENT_VALIDATE_NON_BOOTABLE_FAILURE, &err);
           // XXX This error can be recovered
        }
        PULL_CONTINUE(EVENT_VALIDATE_NON_BOOTABLE_START, NULL);
        err = verify_object(&new_firmware, df, x, y, ef, buffer, BUFFER_SIZE);
        PULL_CONTINUE(EVENT_VALIDATE_NON_BOOTABLE_STOP, NULL);
        if (err) {
           // XXX  invalidate_object(id, &obj_t);
            PULL_CONTINUE(ERROR_VALIDATE_NON_BOOTABLE_INVALID, &err);
           // XXX This error can be recovered
        }

        // (4.2) If valid, move it to the oldest bootable slot
        PULL_CONTINUE(EVENT_UPGRADE, NULL);
        err = get_oldest_firmware(&id_oldest_bootable, &version_bootable, &obj_t, true, false);
        if (err) {
            PULL_CONTINUE(EVENT_UPGRADE_FAILURE, &err);
        }
        err = memory_open(&destination_slot, &id_oldest_bootable, WRITE_ALL);
        if (err) {
            PULL_CONTINUE(EVENT_UPGRADE_FAILURE_2, &err);
        }
        PULL_CONTINUE(EVENT_UPGRADE_COPY_START, NULL);
        err = copy_firmware(&new_firmware, &destination_slot, buffer, BUFFER_SIZE);
        PULL_CONTINUE(EVENT_UPGRADE_COPY_STOP, NULL);
        if (err) {
            PULL_CONTINUE(EVENT_UPGRADE_COPY_FAILURE, &err);
        }
        memory_close(&destination_slot);
        PULL_CONTINUE(EVENT_UPGRADE_SUCCESS, NULL);
        already_validated_flag = true;
        PULL_CONTINUE(EVENT_BOOT, &id_oldest_bootable);
    }
    // (5) Check if the validation has been already performed during the upgrade
    if (!already_validated_flag) {

        // (5.1) Validate the newest_bootable
        PULL_CONTINUE(EVENT_BOOTABLE_VALIDATE_START, NULL);
        err = verify_object(&newest_bootable, df, x, y, ef, buffer, BUFFER_SIZE);
        PULL_CONTINUE(EVENT_BOOTABLE_VALIDATE_STOP, NULL);
        if (!err) {
            PULL_CONTINUE(EVENT_BOOT, &id_newest_bootable);
        } else {
            PULL_CONTINUE(EVENT_BOOTABLE_VALIDATE_FAILURE, &err);

            // (5.1.1) Restore the recovery image if available
            PULL_CONTINUE(EVENT_RECOVERY_RESTORE_START, NULL);
            if (RECOVERY_IMAGE == 0) {
                PULL_CONTINUE(EVENT_RECOVERY_RESTORE_START, NULL);
                err = restore_recovery_image(&id_newest_bootable);
                PULL_CONTINUE(EVENT_RECOVERY_RESTORE_STOP, NULL);
                if (!err) {
                    PULL_CONTINUE(EVENT_BOOT, &id_newest_bootable);
                } else {
                    PULL_CONTINUE(EVENT_FATAL_FAILURE, NULL);
                }
            }
        }
    }
    PULL_FINISH(EVENT_FINISH);
}
