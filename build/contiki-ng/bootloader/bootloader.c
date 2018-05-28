#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "contiki.h"
#include "contiki-lib.h"

#include "driverlib/flash.h"
#include "driverlib/vims.h"
#include "dev/watchdog.h"

#include "bootloader.h"
#include "../memory_headers.h"
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

void pull_bootloader() {
    while(1) {
        agent_msg bootloader_agent();
        if (event == EVENT_STORE_RECOVERY_COPY_START) {
            watchdog_stop();
        } else if (event == EVENT_STORE_RECOVERY_COPY_STOP) {
            watchdog_start();
        } else if (event == EVENT_VALIDATE_NON_BOOTABLE_START) {
            watchdog_stop();
        } else if (event == EVENT_VALIDATE_NON_BOOTABLE_STOP) {
            watchdog_start();
        }
    }
}
