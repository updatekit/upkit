#include "agents/update.h"
#include "memory_file_posix.h"
#include "transport_libcoap.h"
#include "security/sha256.h"

#include "support/sample_data.h"
#include "support/test_runner.h"
#include "unity.h"
#include <unistd.h>

#define FOREACH_TEST(DO) \
    DO(update_udp, 0)
TEST_RUNNER();

#define POLLING_FREQUENCY 1
#define BUFFER_SIZE 1024

DIGEST_FUNC(tinydtls);

void logic(conn_type type, void* conn_data);

void setUp(void) {
    override_memory_object(OBJ_1, "../assets/external_flash_simulator_updated", 0x19000, 0x32000);
    override_memory_object(OBJ_2, "../assets/external_flash_simulator_updated", 0x32000, 0x4B000);
    override_memory_object(OBJ_RUN, "../assets/internal_flash_simulator_updated", 0x7000, 0x20000);
    mem_object obj_t;
    TEST_ASSERT_TRUE(invalidate_object(OBJ_2, &obj_t) == PULL_SUCCESS);
}

void tearDown(void) { }

void test_update_udp(void) {
    logic(CONN_UDP, NULL);
}

// The test logic should update the OBJ_2 with the firmware with version 0xdead
// After the test is finished I invalidate the OBJ_2 to restore the status
void logic(conn_type type, void* conn_data) {
    static agent_t agent;
    static update_agent_config cfg;
    static int8_t retries = 3;
    static uint8_t success = 0;
    static update_agent_ctx_t ctx;

    while (1) {
        agent = update_agent(&cfg, &ctx);
        if (agent.current_error != PULL_SUCCESS) {
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
    if (success) {
        printf("The firmware has been correctly received\n");
    }
}
