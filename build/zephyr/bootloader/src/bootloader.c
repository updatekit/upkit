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
    bootloader_agent_config cfg;
    cfg.bootloader_ctx_id = BOOTLOADER_CTX;
    cfg.swap_id = SWAP;
    cfg.swap_size = SWAP_SIZE;
    bootloader_agent_vendor_keys(&cfg, vendor_x_g, vendor_y_g);
    bootloader_agent_set_buffer(&cfg, buffer, BUFFER_SIZE);

    log_debug("Bootloader started\n");
    void* event_data;
    agent_event_t agent_event = bootloader_agent(&cfg, &event_data);
    if (IS_FAILURE(agent_event)) {
        log_debug("Bootloader error: %s\n", err_as_str(GET_ERROR(event_data)));
        // XXX handle this
    } else if (IS_CONTINUE(agent_event)) {
        if (agent_event == EVENT_BOOT) {
            printf("loading object\n");
            load_object(GET_BOOT_ID(event_data));
        }
    }
    // XXX I still need to manage the fatal errors
}
