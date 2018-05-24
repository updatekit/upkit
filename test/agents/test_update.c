#include "agents/update.h"
#include "memory_file_posix.h"
#include "transport_libcoap.h"
#include "security/sha256.h"

#include "support/sample_data.h"
#include "support/test_runner.h"
#include "unity.h"
#include <unistd.h>

#define FOREACH_TEST(DO) \
    DO(update_success, 0)
TEST_RUNNER();

#define POLLING_FREQUENCY 1
#define BUFFER_SIZE 1024

DIGEST_FUNC(tinydtls);

static agent_t agent;
static update_agent_config cfg;
static update_agent_ctx_t ctx;
static int8_t retries = 3;
static uint8_t success = 0;
static uint8_t buffer[BUFFER_SIZE];

// The test logic should update the OBJ_2 with the firmware with version 0xdead
// After the test is finished I invalidate the OBJ_2 to restore the status
void setUp(void) {
    override_memory_object(OBJ_1, "../assets/external_flash_simulator_updated", 0x19000, 0x32000);
    override_memory_object(OBJ_2, "../assets/external_flash_simulator_updated", 0x32000, 0x4B000);
    override_memory_object(OBJ_RUN, "../assets/internal_flash_simulator_updated", 0x7000, 0x20000);
    mem_object obj_t;
    TEST_ASSERT_TRUE(invalidate_object(OBJ_2, &obj_t) == PULL_SUCCESS);
}

void tearDown(void) { 
    bzero(&agent, sizeof(agent));
    bzero(&cfg, sizeof(cfg));
    bzero(&ctx, sizeof(ctx));
    retries = 3;
    success = 0;
}

void test_update_success(void) {
    conn_config(&cfg.subscriber, "localhost", 0, CONN_UDP, NULL, "version");
    conn_config(&cfg.receiver, "localhost", 0, CONN_UDP, NULL, "firmware");
    update_agent_reuse_connection(&cfg, 0);
    update_agent_set_identity(&cfg, identity_g);
    update_agent_vendor_keys(&cfg, vendor_x_g, vendor_y_g);
    update_agent_digest_func(&cfg, tinydtls_digest_sha256);
    update_agent_ecc_func(&cfg, tinydtls_secp256r1_ecc);
    update_agent_set_buffer(&cfg, buffer, BUFFER_SIZE);

    while (1) {
        printf("CAlling the function\n");
        sleep(1);
        agent = update_agent(&cfg, &ctx);
        if (agent.current_error) {
            if (agent.required_action == FAILURE) {
                break;
            } else if (agent.required_action == RECOVER) {
                if (retries-- <= 0) {
                    continue;
                } else {
                    break;
                }
            }
        }
        if (agent.current_state == STATE_APPLY) {
            success = 1;
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(success, "There was an error during the update phase\n");
}
