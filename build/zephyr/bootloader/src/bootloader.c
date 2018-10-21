#include <libpull_agents/bootloader_agent.h>
#include "default_keys.h"

#define WITH_BOOTLOADER 1
#include "platform_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE 2048
#define SWAP_SIZE 2048

static uint8_t buffer[BUFFER_SIZE];

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

int main(void) {
    agent_msg_t agent_msg;
    bootloader_agent_config cfg;
    cfg.bootloader_ctx_id = BOOTLOADER_CTX;
    cfg.swap_id = SWAP;
    cfg.swap_size = SWAP_SIZE;
    bootloader_agent_vendor_keys(&cfg, vendor_x_g, vendor_y_g);
#if WITH_TINYCRYPT
    cfg.df = tinycrypt_digest_sha256;
    cfg.ef = tinycrypt_secp256r1_ecc;
#endif
    bootloader_agent_set_buffer(&cfg, buffer, BUFFER_SIZE);

    log_debug("Bootloader started\n");
    agent_msg = bootloader_agent(&cfg);
    if (IS_FAILURE(agent_msg)) {
        log_debug("Bootloader error: %s\n", err_as_str(GET_ERROR(agent_msg)));
        // XXX handle this
    } else if (IS_CONTINUE(agent_msg)) {
        if (agent_msg.event == EVENT_BOOT) {
            printf("loading object\n");
            load_object(*((mem_id_t*) agent_msg.event_data));
        }
    }
    // XXX I still need to manage the fatal errors
}
