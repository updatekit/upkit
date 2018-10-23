#include <libpull/security/sha256.h>
#include "libpull_agents/bootloader_agent.h"

#include "support/support.h"
#include <unistd.h>

#define BUFFER_SIZE 1024

static agent_event_t agent_event;
static bootloader_agent_config cfg;

static int8_t retries = 3;
static uint8_t success = 0;
static uint8_t buffer[BUFFER_SIZE];

// The test logic should update the OBJ_2 with the firmware with version 0xdead
// After the test is finished I invalidate the OBJ_2 to restore the status
void ntest_prepare(void) {
    bzero(&cfg, sizeof(cfg));
}

void ntest_clean(void) { 
    restore_assets();
    retries = 3;
    success = 0;
}

void test_bootloader_success(void) {

    bootloader_agent_vendor_keys(&cfg, (uint8_t*) vendor_x_g, (uint8_t*) vendor_y_g);
    bootloader_agent_digest_func(&cfg, tinydtls_digest_sha256);
    bootloader_agent_ecc_func(&cfg, tinydtls_secp256r1_ecc);
    bootloader_agent_set_buffer(&cfg, buffer, BUFFER_SIZE);
    cfg.bootloader_ctx_id = BOOTLOADER_CTX;
    cfg.recovery_id = OBJ_GOLD;
    cfg.swap_size = SWAP_SIZE;
    cfg.swap_id = OBJ_SWAP;

    void* event_data = NULL;
    agent_event = bootloader_agent(&cfg, &event_data);
    if (IS_FAILURE(agent_event)) {
        printf("error is: %s\n", (err_as_str(GET_ERROR(event_data))));
        success = false;
    } else if (IS_CONTINUE(agent_event)) {
        nTEST_TRUE(agent_event == EVENT_BOOT);
        nTEST_COMPARE_INT(OBJ_A, GET_BOOT_ID(event_data));
        success = true;
    }
    nTEST_TRUE(success, "Error during the booting process");
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_bootloader_success);
    nTEST_END();
    nTEST_RETURN();
}
