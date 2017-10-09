#include "unity.h"

#include "logger.h"
#include "error.h"
#include "metadata.h"
#include "simple_metadata.h"
#include "receiver.h"
#include "subscriber.h"
#include "testing_settings.h"
#include "memory_file_posix.h"
#include "transport_libcoap.h"
#include "async_libcoap.h"
#include "memory_objects.h"
#include "sample_data.h"
#include "ecc.h"
#include "sha256.h"
#include "verifier.h"
#include "tinydtls.h"
#include "memory.h"

#include <unistd.h>

#define POLLING_FREQUENCY 1


void logic(conn_type type, void* conn_data);

void setUp(void) {
    override_memory_object(OBJ_1, "assets/external_flash_simulator_updated", 0x19000, 0x32000);
    override_memory_object(OBJ_2, "assets/external_flash_simulator_updated", 0x32000, 0x4B000);
    override_memory_object(OBJ_RUN, "assets/internal_flash_simulator_updated", 0x7000, 0x20000);
}

void tearDown(void) {
    metadata invalid_mt;
    bzero(&invalid_mt, sizeof(metadata));
    mem_object obj_t;
    pull_error err = write_firmware_metadata(OBJ_2, &invalid_mt, &obj_t);
    TEST_ASSERT_TRUE(!err);
}

void test_logic_udp(void) {
    logic(UDP, NULL);
}

void test_logic_dtls(void) {
    // Set up dtls ecdh connection
    static dtls_ecdh_data_t ecdh_data = {
        .curve = CURVE_SECP256R1,
        .priv_key = (uint8_t*) priv_g,
        .pub_key_x = (uint8_t*) x,
        .pub_key_y = (uint8_t*) y
    };
    logic(DTLS_ECDH, &ecdh_data);
}

// The test logic should update the OBJ_2 with the firmware with version 0xdead
// After the test is finished I invalidate the OBJ_2 to restore the status
void logic(conn_type type, void* conn_data) {
    subscriber_ctx sctx;
    txp_ctx stxp;

    receiver_ctx rctx;
    txp_ctx rtxp;
    obj_id id;
    mem_object obj_t;

    txp_init(&stxp, PROV_SERVER, 0, type, conn_data);

    log_info("Subscribing to the server\n");
    pull_error err = subscribe(&sctx, &stxp, "version", &obj_t);
    TEST_ASSERT_TRUE(!err);
    while(1) {
        // Check if there are updates
        log_info("Checking for updates\n");
        while (!sctx.has_updates) {
            check_updates(&sctx, subscriber_cb); // check for errors
            loop_once(&stxp, 1000);
            sleep(POLLING_FREQUENCY);
        }
        log_info("There is an update\n");
        // get the oldest slot
        uint16_t version;
        err = get_oldest_firmware(&id, &version, &obj_t);
        TEST_ASSERT_TRUE(!err);
        // download the image to the oldest slot
        err = txp_init(&rtxp, PROV_SERVER, 0, type, conn_data);
        TEST_ASSERT_TRUE(!err);
        err = receiver_open(&rctx, &rtxp, "firmware", id, &obj_t);
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
        err = verify_object(id, digest_sha256, x, y, secp256r1, &obj_t);
        TEST_ASSERT_TRUE(!err);
        break;
    }
    unsubscribe(&sctx);
    txp_end(&stxp);
}
