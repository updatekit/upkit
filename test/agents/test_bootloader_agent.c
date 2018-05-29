#include <libpull/security/sha256.h>
#include "libpull_agents/bootloader_agent.h"

#include "support/sample_data.h"
#include "support/test_runner.h"
#include "unity.h"
#include <unistd.h>

#define FOREACH_TEST(DO) \
    DO(bootloader_success, 0)

TEST_RUNNER();

#define BUFFER_SIZE 1024

static agent_msg_t agent_msg;
static bootloader_agent_config cfg;

static int8_t retries = 3;
static uint8_t success = 0;
static uint8_t buffer[BUFFER_SIZE];

// The test logic should update the OBJ_2 with the firmware with version 0xdead
// After the test is finished I invalidate the OBJ_2 to restore the status
void setUp(void) {
    bzero(&cfg, sizeof(cfg));
}

void tearDown(void) { 
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
    while(1) {
        agent_msg = bootloader_agent(&cfg);
        if (IS_FAILURE(agent_msg)) {
            printf("error is:%s\n", (err_as_str(GET_ERROR(agent_msg))));
            success = false;
            break;
        } else if (IS_CONTINUE(agent_msg)) {
            if (agent_msg.event == EVENT_BOOT) {
                TEST_ASSERT_TRUE(agent_msg.event == EVENT_BOOT);
                TEST_ASSERT_EQUAL_INT(OBJ_A, GET_BOOT_ID(agent_msg));
                success = true;
                break;
            }
            continue;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(success, "Error during the booting process");
}
