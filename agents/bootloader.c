#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/security.h>

static manifest_t mt;
static bootloader_ctx ctx;
static mem_id_t id;

static mem_object_t bootloader_object;
static mem_object_t obj_t;
static mem_object_t internal_object;
static mem_object_t new_firmware;

static uint8_t already_validated_flag = 0;

pull_error restore_recovery_image() {
    // TODO implement
    return GENERIC_ERROR;
}


agent_msg_t bootloader_agent() {
    // (1) Open the bootloader context
    log_debug("Loading bootloader context\n");
    err = memory_open(&bootloader_object, BOOTLOADER_CTX, WRITE_CHUNK);
    if (err) {
        goto error;
    }
    if (memory_read(&bootloader_object, &ctx, sizeof(ctx), 0x0) != sizeof(ctx)) {
        err = MEMORY_READ_ERROR;
        goto error;
    }
    if ((ctx.startup_flags & FIRST_BOOT) == FIRST_BOOT) {
        state = ERASE_SLOTS;
        log_debug("Erasing slots\n");
        mem_id_t i;
        for (i = 0; memory_objects[i] >= 0; i++) {
            err = invalidate_object(memory_objects[i], &obj_t);
            if (err) {
                goto error;
            }
        }
#if RECOVERY_IMAGE == 1
        state = STORE_RECOVERY_IMAGE;
        log_debug("Storing Recovery Image\n");
        // Open OBJ_GOLD
        err = memory_open(&obj_gold, OBJ_GOLD, WRITE_ALL);
        if (err) {
            goto err;
        }
        watchdog_stop();
        err = copy_firmware(&internal_object, &obj_gold, buffer, BUFFER_SIZE);
        watchdog_start();
        if (err) {
            goto error;
        }
#endif
        state = WRITE_BOOTLOADER_CTX;
        ctx.startup_flags = 0x0;
        if (memory_write(&bootloader_object, &ctx, sizeof(ctx), 0x0) != sizeof(ctx)) {
            goto error;
        };
        log_debug("Bootstrap Success\n");
    }
    memory_close(&bootloader_object); // close bootloader context
    // (2) Get the newest bootable firmware (there must be at least one)
    state = GET_NEWEST_FIRMWARE;
    err = get_newest_firmware(&id, &version, &obj_t, true, true);
    if (err) {
        goto error;
    }
    // (3) Get the newest non bootable firmware
    err = get_newest_firmware(&id, &version, &obj_t, true, false);
    if (err) {
        goto error;
    }
    // (4) Compare them and upgrade if necessary
    if (version > get_version(&mt)) {
        mem_id_t from = id;
        mem_id_t to;
        // (4.1) Get the oldest bootable slot
        err = get_oldest_firmware(&to, &version, &obj_t, true, false);
        if (err) {
            goto error;
        }
        err = memory_open(&obj_t, &from, READ_ONLY);
        if (err) {
            goto error;
        }
        // (4.2) Validate from image
        // I'm performing the validation before the copy, since if the object
        // is invalid it does not make sense to copy it into the internal
        // memory. Moreover, first copying and then validating the image would
        // require to invalidate the CPU cache that contains part of the
        // internal memory.
        state = VALIDATE_EXTERNAL_IMAGE;
        watchdog_stop();
        err = verify_object(&new_firmware, df, x, y, ef, buffer, BUFFER_SIZE);
        watchdog_start();
        if (err) {
            invalidate_object(id, &obj_t);
            goto error;
        }
        /*********** UPDATE_FIRMWARE **********/
        // (4.3) Update the firmware
        state = UPDATE_FIRMWARE;
        mem_object_t internal_object_write; // not hielding, so it's safe!
        err = memory_open(&internal_object_write, OBJ_RUN, WRITE_ALL);
        if (err) {
            goto error;
        }
        err = copy_firmware(&new_firmware, &internal_object_write, buffer, BUFFER_SIZE);
        memory_close(&internal_object_write);
        if (err) {
            goto error;
        }
        already_validated_flag = 1;
    }

    watchdog_stop();
    if (!already_validated_flag) {
        err = verify_object(&internal_object, df, x, y, ef, buffer, BUFFER_SIZE);
    }
    watchdog_start();
    if (err) {
        if (RECOVERY_IMAGE == 0 || restore_recovery_image() != PULL_SUCCESS) {
            goto fatal_error;
        }
    }
}
