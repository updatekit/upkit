#include "common/libpull.h"
#include "network/receiver.h"
#include "network/subscriber.h"
#include "network/async.h"
#include "security/ecc.h"
#include "security/sha256.h"
#include "security/verifier.h"
#include "memory/memory_objects.h"
#include "memory/memory.h"
#include "memory/manifest.h"

#include "memory_file_posix.h"
#include "transport_libcoap.h"

#include "support/sample_data.h"
#include "support/test_runner.h"
#include "unity.h"
#include <unistd.h>

#define FOREACH_TEST(DO) \
    DO(logic_udp, 0) \
    DO(logic_dtls,0)

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

void tearDown(void) {
}

void test_logic_udp(void) {
    logic(CONN_UDP, NULL);
}

void test_logic_dtls(void) {
    // Set up dtls ecdh connection
    static dtls_ecdh_data_t ecdh_data = {
        .priv_key = (uint8_t*) dtls_client_priv_g,
        .pub_key_x = (uint8_t*) dtls_client_x_g,
        .pub_key_y = (uint8_t*) dtls_client_y_g
    };
    logic(CONN_DTLS_ECDH, &ecdh_data);
}

// The test logic should update the OBJ_2 with the firmware with version 0xdead
// After the test is finished I invalidate the OBJ_2 to restore the status
void logic(conn_type type, void* conn_data) {
    subscriber_ctx sctx;
    txp_ctx stxp;

    receiver_ctx rctx;
    txp_ctx rtxp;
    obj_id id;
    mem_object new_obj;
    mem_object obj_t;

    uint8_t buffer[BUFFER_SIZE];

    txp_init(&stxp, "localhost", 0, type, conn_data);

    log_info("Subscribing to the server\n");
    pull_error err = subscribe(&sctx, &stxp, "version", &obj_t);
    TEST_ASSERT_TRUE(!err);
    while(1) {
        // Check if there are updates
        log_info("Checking for updates\n");
        while (!sctx.has_updates) {
            TEST_ASSERT_TRUE(!check_updates(&sctx, subscriber_cb));
            loop_once(&stxp, 1000);
            sleep(POLLING_FREQUENCY);
        }
        log_info("There is an update\n");
        // get the oldest slot
        uint16_t version;
        err = get_oldest_firmware(&id, &version, &obj_t);
        TEST_ASSERT_TRUE(!err);
        // download the image to the oldest slot
        err = txp_init(&rtxp, "localhost", 0, type, conn_data);
        TEST_ASSERT_TRUE(!err);
        // open destination object
        err = memory_open(&new_obj, id, WRITE_ALL);
        TEST_ASSERT_TRUE(!err);
        err = receiver_open(&rctx, &rtxp, identity_g, "firmware", &new_obj);
        TEST_ASSERT_TRUE(!err);
        while (!rctx.firmware_received) {
            err= receiver_chunk(&rctx);
            TEST_ASSERT_TRUE(!err);
            loop(&rtxp, 1000);
        }
        txp_end(&rtxp);
        err = receiver_close(&rctx);
        TEST_ASSERT_TRUE(!err);
        TEST_ASSERT_TRUE(rctx.firmware_received);
        err = verify_object(&new_obj, tinydtls_digest_sha256, vendor_x_g, vendor_y_g, tinydtls_secp256r1_ecc, 
                                     buffer, BUFFER_SIZE);
        memory_close(&new_obj);
        TEST_ASSERT_TRUE(!err);
        break;
    }
    unsubscribe(&sctx);
    txp_end(&stxp);
}
