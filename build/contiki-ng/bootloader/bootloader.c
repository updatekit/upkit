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

#define START_PAGE 5
#define END_PAGE 31

#define RECOVERY_IMAGE 0
#define MEMORY_OBJ_BUFFER_SIZE 0x100
#define FLASH_PAGE_SIZE 0x1000

#ifdef WITH_CRYPTOAUTHLIB
#include <cryptoauthlib.h>
DIGEST_FUNC(cryptoauthlib);
#elif WITH_TINYDTLS
DIGEST_FUNC(tinydtls);
#elif WITH_TINYCRYPT
DIGEST_FUNC(tinycrypt);
// To perform the verification I do not need any rng. Define e dummy
// function to make tinycrypt happy.
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

/**** TODO update the states */
enum bootloader_states {
    OPEN_INTERNAL_IMAGE = 0,
    READ_BOOTLOADER_CTX,
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
static obj_id id;

uint8_t buffer[MEMORY_OBJ_BUFFER_SIZE];

static mem_object bootloader_object;
static mem_object obj_t;
static mem_object internal_object;
static mem_object new_firmware;

static digest_func df;
static ecc_func_t ef;

void specialize_crypto_functions() {
#if WITH_CRYPTOAUTHLIB
    df = cryptoauthlib_digest_sha256;
    ef = cryptoauthlib_ecc;
#elif WITH_TINYDTLS
    df = tinydtls_digest_sha256;
    ef = tinydtls_secp256r1_ecc;
#elif WITH_TINYCRYPT
    df = tinycrypt_digest_sha256;
    ef = tinycrypt_secp256r1_ecc;
#endif
}

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
    specialize_crypto_functions();
    /************ OPEN INTERNAL IMAGE **********/
    state = OPEN_INTERNAL_IMAGE;
    pull_error err = memory_open(&internal_object, OBJ_RUN);
    if (err) {
        goto error;
    }
    /************ READ_BOOTLOADER_CTX **********/
    state = READ_BOOTLOADER_CTX;
    log_debug("Loading bootloader context\n");
    err = memory_open(&bootloader_object, BOOTLOADER_CTX);
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
        obj_id i;
        for (i = 0; memory_objects[i] >= 0; i++) {
            err = invalidate_object(memory_objects[i], &obj_t);
            if (err) {
                goto error; 
            }
        }
#if RECOVERY_IMAGE
        state = STORE_RECOVERY_IMAGE;
        log_debug("Storing Recovery Image\n");
        // Open OBJ_GOLD
        err = memory_open(&obj_gold, OBJ_GOLD);
        if (err) {
            goto err;
        }
        watchdog_stop();
        err = copy_firmware(&internal_object, &obj_gold, buffer, MEMORY_OBJ_BUFFER_SIZE);
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
    /************ READ_RUNNING_MANIFEST ************/
    state  = READ_RUNNING_MANIFEST;
    log_debug("Reading manifest of the internal image\n");
    err = read_firmware_manifest(&internal_object, &mt);
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
        /*********** OPEN NEW IMAGE *********/
        err = memory_open(&new_firmware, id);
        if (err) {
            goto error;
        }
        /*********** VALIDATE_EXTERNAL_IMAGE *********/
        state = VALIDATE_EXTERNAL_IMAGE;
        err = verify_object(&new_firmware, df, x, y, ef, buffer, MEMORY_OBJ_BUFFER_SIZE);
        if (err) {
            goto error;    
        }
        /*********** UPDATE_FIRMWARE **********/
        state = UPDATE_FIRMWARE;
        int page = 0;
        for (page=START_PAGE; page<=END_PAGE; page++) { // XXX hardcoded pages
            if (FlashSectorErase(FLASH_PAGE_SIZE*page) != FAPI_STATUS_SUCCESS) {
                log_info("Error erasing page %d\n", page);
                goto error;
            }
        }
        err = copy_firmware(&new_firmware, &internal_object, buffer, MEMORY_OBJ_BUFFER_SIZE);
        if (err) {
            goto error;
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
    err = verify_object(&internal_object, df, x, y, ef, buffer, MEMORY_OBJ_BUFFER_SIZE);
    watchdog_start();
    if (err) {
        if (RECOVERY_IMAGE == 0 || restore_recovery_image() != PULL_SUCCESS) {
            goto fatal_error;
        }
    }
#ifdef WITH_CRYPTOAUTHLIB
    if (!err) {
        atcab_release();
    }
#endif
    state = BOOT;
    flash_write_protect();
    load_object(OBJ_RUN, &mt); // refactor this function
fatal_error:
    /* If you arrive here, you should reboot and try again */
    log_info("The bootloader encountered a fatal error");
}
