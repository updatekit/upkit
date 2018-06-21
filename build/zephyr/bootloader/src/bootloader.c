#include <libpull_agents/bootloader_agent.h>
#include "default_keys.h"

#define BOOTLOADER
#include "platform_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE 2048
#define SWAP_SIZE 2048

static uint8_t buffer[BUFFER_SIZE];

static digest_func df;
static ecc_func_t ef;

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

void specialize_crypto_functions() {
#if WITH_TINYCRYPT
    df = tinycrypt_digest_sha256;
    ef = tinycrypt_secp256r1_ecc;
#endif
}

int main(void) {
    agent_msg_t agent_msg;
    bootloader_agent_config cfg = {
        .bootloader_ctx_id = BOOTLOADER_CTX,
        .swap_id = SWAP,
        .swap_size = SWAP_SIZE
    };
    specialize_crypto_functions();
    bootloader_agent_vendor_keys(&cfg, vendor_x_g, vendor_y_g);
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
            if (agent_msg.event == EVENT_BOOT) {
                printf("loading object\n");
                load_object(*((mem_id_t*) agent_msg.event_data));
            }
        }
        // XXX I still need to manage the fatal errors
    }
}
