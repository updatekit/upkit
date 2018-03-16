#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "driverlib/flash.h"
#include "driverlib/vims.h"
#include "dev/watchdog.h"

#include "common/libpull.h"
#include "memory/manifest.h"
#include "memory/memory_objects.h"
#include "memory/memory.h"
#include "security/verifier.h"
#include "security/sha256.h"

#include "bootloader.h"
#include "../memory_headers.h"
#include "../default_configs.h"

#ifdef WITH_CRYPTOAUTHLIB
#include <cryptoauthlib.h>
#endif

#define RECOVERY_IMAGE 0
#define MEMORY_OBJ_BUFFER_SIZE 0x100
#define FLASH_PAGE_SIZE 0x1000

DIGEST_FUNC(cryptoauthlib);

enum bootloader_states {
    READ_BOOTLOADER_CTX = 0,
    ERASE_SLOTS,
    STORE_RECOVERY_IMAGE,
    WRITE_BOOTLOADER_CTX,
    READ_RUNNING_MANIFEST,
    GET_NEWEST_FIRMWARE,
    VALIDATE_EXTERNAL_IMAGE,
    UPDATE_FIRMWARE,
    VALIDATE_IMAGE,
    BOOT
};

static enum bootloader_states state;
static manifest_t mt;
static bootloader_ctx ctx;
static mem_object obj;
static obj_id id;
static mem_object obj_t, obj_dst_t;
uint8_t buffer[MEMORY_OBJ_BUFFER_SIZE];

static const obj_id internal_firmware = OBJ_RUN;


pull_error restore_recovery_image() {
    // TODO implement
    return GENERIC_ERROR;
}

void flash_write_protect() {
    uint32_t page = 0;
    for (page=0; page <= 31; page++) {
        FlashProtectionSet(page, FLASH_WRITE_PROTECT);
    }
}

void pull_bootloader() {
    /************ READ_BOOTLOADER_CTX **********/
    state = READ_BOOTLOADER_CTX;
    log_debug("Loading bootloader context\n");
    pull_error err = memory_open(&obj, BOOTLOADER_CTX);
    if (err) {
        goto error;
    }
    if (memory_read(&obj, &ctx, sizeof(ctx), 0x0) != sizeof(ctx)) {
        err = MEMORY_READ_ERROR;
        goto error;
    }
    if ((ctx.startup_flags & FIRST_BOOT) == FIRST_BOOT) {
        state = ERASE_SLOTS;
        log_debug("Erasing slots\n");
        obj_id i;
        memset(&mt, 0x0, sizeof(mt));
        for (i = 0; memory_objects[i] >= 0; i++) {
            err = write_firmware_manifest(memory_objects[i], &mt, &obj_t);
            if (err) {
                goto error; 
            }
        }
#if RECOVERY_IMAGE
        state = STORE_RECOVERY_IMAGE;
        log_debug("Storing Recovery Image\n");
        watchdog_stop();
        err = copy_firmware(internal_firmware, OBJ_GOLD, &obj_t, &obj_dst_t, buffer, MEMORY_OBJ_BUFFER_SIZE);
        watchdog_start();
        if (err) {
            goto error;
        }
#endif
        state = WRITE_BOOTLOADER_CTX;
        ctx.startup_flags = 0x0;
        if (memory_write(&obj, &ctx, sizeof(ctx), 0x0) != sizeof(ctx)) {
            goto error;
        };
        log_debug("Bootstrap Success\n");
    }
    memory_close(&obj); // close bootloader context
    /************ READ_RUNNING_MANIFEST ************/
    state  = READ_RUNNING_MANIFEST;
    log_debug("Reading manifest of the internal image\n");
    err = read_firmware_manifest(internal_firmware, &mt, &obj_t);
    if (err) {
        goto error;
    }
    uint16_t version = 0; 
    /************ GET_NEWEST_FIRMWARE *************/
    state = GET_NEWEST_FIRMWARE;
    err = get_newest_firmware(&id, &version, &obj_t);
    if (err) {
        goto error;
    }
    log_debug("Newest firmware is in slot %u with version %x\n", id, version);
    if (version > get_version(&mt)) {
        /*********** VALIDATE_EXTERNAL_IMAGE *********/
        state = VALIDATE_EXTERNAL_IMAGE;
        err = verify_object(id, cryptoauthlib_digest_sha256, x, y, cryptoauthlib_ecc, 
                &obj_t, buffer, MEMORY_OBJ_BUFFER_SIZE);
        if (err) {
            goto error;    
        }
        /*********** UPDATE_FIRMWARE **********/
        state = UPDATE_FIRMWARE;
        int page = 0;
        for (page=5; page<=5+26; page++) { // XXX hardcoded pages
            if (FlashSectorErase(FLASH_PAGE_SIZE*page) != FAPI_STATUS_SUCCESS) {
                log_info("Error erasing page %d\n", page);
                goto error;
            }
        }
        if (page > 31) {
            err = copy_firmware(id, internal_firmware, &obj_t, &obj_dst_t, buffer, MEMORY_OBJ_BUFFER_SIZE);
            if (err) {
                goto error;
            }
        }
    }
    goto boot;
error:
    log_error(err, "The bootloader encountered an error in phase %d\n", state);
boot:
    log_info("Booting");
    /************ VALIDATING_IMAGE *********/
#ifdef WITH_CRYPTOAUTHLIB
    ATCA_STATUS status = atcab_init(&cfg_ateccx08a_i2c_default);
    if (status != ATCA_SUCCESS) {
        // XXX define a specific error for this case
        goto fatal_error;
    }
#endif
    watchdog_stop();
    err = verify_object(internal_firmware, cryptoauthlib_digest_sha256, x, y, cryptoauthlib_ecc,
                                    &obj_t, buffer, MEMORY_OBJ_BUFFER_SIZE);
    watchdog_start();
    if (err) {
        if (RECOVERY_IMAGE == 0 || restore_recovery_image() != PULL_SUCCESS) {
            goto fatal_error;
        }
    }
#ifdef WITH_CRYPTOAUTHLIB
    if (status == ATCA_SUCCESS) {
        atcab_release();
    }
#endif
    state = BOOT;
    flash_write_protect();
    load_object(internal_firmware, &mt);
fatal_error:
    /* If you arrive here, you should reboot and try again */
    log_info("The bootloader encountered a fatal error");
}
