#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "driverlib/flash.h"
#include "driverlib/vims.h"
#include "dev/watchdog.h"

#include "memory/metadata.h"
#include "memory/memory_objects.h"
#include "memory/memory.h"
#include "common/loader.h"
#include "common/error.h"
#include "security/verifier.h"
#include "security/sha256.h"

#include "common/external.h"
#include "bootloader.h"
#include "../memory_headers.h"

#include "common/logger.h"

static const uint8_t x[32] = {
    0x8b,0x27,0x39,0x67,0x01,0x4b,0x1c,0xae,0xfe,0x8a,0x18,0x6e,0xea,0x27,0x86,0x34,
    0x0e,0xea,0x35,0x3d,0x8c,0x65,0xf6,0x59,0xfc,0xcb,0x23,0xd7,0xfa,0xab,0x7b,0x18
};
static const uint8_t y[32] = {
    0x14,0x75,0x33,0xec,0x17,0xb7,0x54,0x50,0xca,0x98,0x35,0xad,0x58,0xbe,0xd5,0xfa,
    0x48,0xbc,0xa0,0x24,0x81,0xba,0xfa,0x3d,0xcd,0x8d,0x5a,0x7f,0x40,0xbc,0x70,0x94
};

#define GOLD_IMAGE 1

static metadata mt;
static bootloader_ctx ctx;
static mem_object obj;
static obj_id id;
static mem_object obj_t, obj_dst_t;

static const obj_id internal_firmware = OBJ_RUN;

void pull_bootloader() {
    log_info("Bootloader started\n");
    log_debug("Loading bootloader context\n");
    pull_error err = memory_open(&obj, BOOTLOADER_CTX);
    if (err) {
        log_error(err, "Failed opening Bootoader context\n");
        goto error;
    }
    if (memory_read(&obj, &ctx, sizeof(ctx), 0x0) != sizeof(ctx)) {
        err = MEMORY_READ_ERROR;
        log_error(err, "Failed reading Bootoader context\n");
        goto error;
    }
    if ((ctx.startup_flags & FIRST_BOOT) == FIRST_BOOT) {
        log_info("Bootstrapping system\n");
        log_info("Erasing slots\n");
        obj_id i;
        memset(&mt, 0x0, sizeof(mt));
        for (i = 0; memory_objects[i] >= 0; i++) {
            err = write_firmware_metadata(memory_objects[i], &mt, &obj_t);
            if (err) {
                log_error(err, "Error erasing firmware\n");
                goto error; 
            }
        }
#if GOLD_IMAGE
        log_info("Storing gold image\n");
        watchdog_stop();
        err = copy_firmware(internal_firmware, OBJ_GOLD, &obj_t, &obj_dst_t);
        if (err) {
            log_error(err, "Failure while storing golden image\n");
            goto error;
        }
        watchdog_start();
#endif

        ctx.startup_flags = 0x0;
        if (memory_write(&obj, &ctx, sizeof(ctx), 0x0) != sizeof(ctx)) {
            log_warn("Error writing bootloader context\n");
            goto error;
        };
        log_info("Bootstrap success\n");
    }
    memory_close(&obj);
    log_debug("Reading metadata of the internal image\n");
    err = read_firmware_metadata(internal_firmware, &mt, &obj_t);
    if (err) {
        log_error(err, "Failed reading internal firmware metadata\n");
        goto error;
    }
    log_info("Internal Firmware: %u\n", get_version(&mt));
    uint16_t version = 0; 
    err = get_newest_firmware(&id, &version, &obj_t);
    if (err) {
        log_error(err, "Failed getting newest firmware\n");
        goto error;
    }
    log_debug("Newest firmware is in slot %u with version %x\n", id, version);
    if (version > get_version(&mt)) {
        log_info("Updating system to version: %x\n", version);
        int page = 0;
        //uint32_t OldVIMSState = VIMSModeGet(VIMS_BASE);
        //VIMSModeSet(VIMS_BASE, VIMS_CTL_MODE_GPRAM);
        for (page=5; page<=5+26; page++) {
            if (FlashSectorErase(0x1000*page) != FAPI_STATUS_SUCCESS) {
                log_info("Error erasing page %d\n", page);
            }
        }
        if (page > 31) {
            err = copy_firmware(id, internal_firmware, &obj_t, &obj_dst_t);
            if (err) {
                log_error(err, "Failure updating to the new firmware\n");
            }
        }
        //VIMSModeSet(VIMS_BASE, OldVIMSState);
    }
error:
    log_info("Validating internal image\n");
    watchdog_stop();
    err = verify_object(internal_firmware, digest_sha256, x, y, secp256r1, &obj_t);
    if (err) {
        log_error(err, "Verification failed\n");
    }
    else {
        log_info("Verification successfull\n");
    }
    watchdog_start();
    log_info("Load the internal firmware\n");
    load_object(internal_firmware, &mt);
}
