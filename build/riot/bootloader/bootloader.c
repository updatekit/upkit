#include <libpull_agents/bootloader_agent.h>
#include "platform_headers.h"
#include "default_keys.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE 2048
#define SWAP_SIZE 2048

static agent_event_t agent_event;
static bootloader_agent_config cfg;
static uint8_t buffer[BUFFER_SIZE];

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

int main(void) {
    agent_event_t agent_event;
    void* event_data = NULL;
    bootloader_agent_set_buffer(&cfg, buffer, BUFFER_SIZE);
    cfg.safestore = safestore_g;
    cfg.bootloader_ctx_id = BOOTLOADER_CTX;
    cfg.swap_size = SWAP_SIZE;
    cfg.swap_id = SWAP;

    log_debug("Bootloader started\n");
    agent_event = bootloader_agent(&cfg, &event_data);
    if (IS_CONTINUE(agent_event) && agent_event == EVENT_BOOT) {
        printf("loading object\n");
        load_object(GET_BOOT_ID(event_data));
    } else if (IS_FAILURE(agent_event)) {
        log_debug("Bootloader error: %s\n", err_as_str(GET_ERROR(agent_event)));
    }
    while(1);
}
