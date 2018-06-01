#include <libpull_agents/bootloader_agent.h>
#include "contiki.h"
#include "contiki-lib.h"
#include "dev/watchdog.h"
#include "platform_headers.h"
#include "../default_configs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE PAGE_SIZE // Defined in Makefile.conf

uint8_t buffer[BUFFER_SIZE];

static digest_func df;
static ecc_func_t ef;

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

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

PROCESS(bootloader, "bootloader process");
AUTOSTART_PROCESSES(&bootloader);

static agent_msg_t agent_msg;

PROCESS_THREAD(bootloader, ev, data) {
    PROCESS_BEGIN();
    bootloader_agent_config cfg = {
        .bootloader_ctx_id = BOOTLOADER_CTX,
    };
    specialize_crypto_functions();
    bootloader_agent_vendor_keys(&cfg, x, y);
    bootloader_agent_digest_func(&cfg, df);
    bootloader_agent_ecc_func(&cfg, ef);
    bootloader_agent_set_buffer(&cfg, buffer, BUFFER_SIZE);

    log_debug("Bootloader started\n");
    while(1) {
        agent_msg = bootloader_agent(&cfg);
        if (IS_FAILURE(agent_msg)) {
            log_debug("Bootloader error: %s\n", err_as_str(GET_ERROR(agent_msg)));
            break;
        } else if (IS_CONTINUE(agent_msg)) {
            if (agent_msg.event == EVENT_VALIDATE_NON_BOOTABLE_START) {
                WATCHDOG_STOP();
            } else if (agent_msg.event == EVENT_VALIDATE_NON_BOOTABLE_STOP) {
                WATCHDOG_START();
            } else if (agent_msg.event == EVENT_VALIDATE_BOOTABLE_START) {
                WATCHDOG_STOP();
            } else if (agent_msg.event == EVENT_VALIDATE_BOOTABLE_STOP) {
                WATCHDOG_START();
            } else if (agent_msg.event == EVENT_UPGRADE_COPY_START) {
                WATCHDOG_STOP();
            } else if (agent_msg.event == EVENT_UPGRADE_COPY_STOP) {
                WATCHDOG_START();
#if RECOVERY_IMAGE
            } else if (agent_msg.event == EVENT_STORE_RECOVERY_COPY_START) {
                WATCHDOG_STOP();
            } else if (agent_msg.event == EVENT_STORE_RECOVERY_COPY_STOP) {
                WATCHDOG_START();
#endif
            } else if (agent_msg.event == EVENT_BOOT) {
                load_object(*((mem_id_t*) agent_msg.event_data));
            break;
            }
            continue;
        }
        // XXX I still need to manage the fatal errors
    }
    PROCESS_END();
}
