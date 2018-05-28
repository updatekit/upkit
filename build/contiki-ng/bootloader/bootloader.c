#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "contiki.h"
#include "contiki-lib.h"

#include "driverlib/flash.h"
#include "driverlib/vims.h"
#include "dev/watchdog.h"

#include <libpull_agents/bootloader_agent.h>

#include "platform_headers.h"
#include "../default_configs.h"

#define BUFFER_SIZE PAGE_SIZE // Defined in Makefile.conf

uint8_t buffer[BUFFER_SIZE];

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

void flash_write_protect() {
    uint32_t page = 0;
    for (page=BOOTLOADER_START_PAGE; page<=IMAGE_END_PAGE; page++) {
        FlashProtectionSet(page, FLASH_WRITE_PROTECT);
    }
}

static agent_msg_t agent_msg;

/// XXX this needs to be fixed
const version_t running_version = 0x0;

void pull_bootloader() {
    bootloader_agent_config cfg = {
        .bootloader_ctx_id = BOOTLOADER_CTX,
        .recovery_id = OBJ_GOLD
    };
    while(1) {
        agent_msg = bootloader_agent(&cfg);
        if (IS_FAILURE(agent_msg)) {
            break;
        } else if (IS_CONTINUE(agent_msg)) {
            if (agent_msg.event == EVENT_STORE_RECOVERY_COPY_START) {
                watchdog_stop();
            } else if (agent_msg.event == EVENT_STORE_RECOVERY_COPY_STOP) {
                watchdog_start();
            } else if (agent_msg.event == EVENT_VALIDATE_NON_BOOTABLE_START) {
                watchdog_stop();
            } else if (agent_msg.event == EVENT_VALIDATE_NON_BOOTABLE_STOP) {
                watchdog_start();
            } else if (agent_msg.event == EVENT_BOOT) {
                load_object(*((mem_id_t*) agent_msg.event_data));
            }
            continue;
        }
    }
}
