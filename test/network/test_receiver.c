#include "common/libpull.h"
#include "network/receiver.h"
#include "memory/manifest.h"
#include "memory/memory_objects.h"
#include "network/transport.h"
#include "network/async.h"

#include "memory_file_posix.h" // Posix
#include "transport_libcoap.h"
#include "async_libcoap.h"

#include "test_runner.h" // Test Support
#include "sample_data.h" // Test Support
#include <unity.h>

#define FOREACH_TEST(DO) \
    DO(get_firmware, 0) \
    DO(receiver_chunk_invalid_transport, 0) \
    DO(get_firmware_invalid_resource, 0) \
    DO(get_firmware_dtls, 0)
TEST_RUNNER();

#define PROV_SERVER "localhost"

mem_object_t obj;

void setUp(void) {
    TEST_ASSERT_TRUE(memory_open(&obj, OBJ_2, WRITE_ALL) == PULL_SUCCESS);
}

void tearDown(void) {
    manifest_t invalid_mt;
    /* Restore state */
    bzero(&invalid_mt, sizeof(manifest_t));
    pull_error err = write_firmware_manifest(&obj, &invalid_mt);
    TEST_ASSERT_TRUE(!err);
}

void test_get_firmware(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, PULL_UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    err = receiver_open(&rcv, &txp, identity_g, "firmware", &obj);
    while (!rcv.firmware_received) {
        err = receiver_chunk(&rcv);
        TEST_ASSERT_TRUE(!err);
        loop(&txp, 1000);
    }
    err = receiver_close(&rcv);
    TEST_ASSERT_TRUE(!err);
    txp_end(&txp);
}

void test_get_firmware_dtls(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    dtls_ecdh_data_t ecdh_data = {
        .priv_key = (uint8_t *) dtls_client_priv_g,
        .pub_key_x = (uint8_t *) dtls_client_x_g,
        .pub_key_y = (uint8_t *) dtls_client_y_g
    };
    pull_error err = txp_init(&txp, PROV_SERVER, 0, PULL_DTLS_ECDH, &ecdh_data);
    TEST_ASSERT_TRUE(!err);
    err = receiver_open(&rcv, &txp, identity_g, "firmware", &obj);
    printf("Starting receiving the firmware\n");
    while (!rcv.firmware_received) {
        err = receiver_chunk(&rcv);
        TEST_ASSERT_TRUE(!err);
        loop(&txp, 1000);
    }
    printf("firmware downloaded\n");
    err = receiver_close(&rcv);
    TEST_ASSERT_TRUE(!err);
    txp_end(&txp);
}

void test_receiver_chunk_invalid_transport(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, PULL_UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    err = receiver_open(&rcv, &txp, identity_g, "firmware", &obj);
    TEST_ASSERT_TRUE(!err);
    rcv.txp = NULL;
    err = receiver_chunk(&rcv);
    TEST_ASSERT_TRUE(err);
}

void test_get_firmware_invalid_resource(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, PULL_UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    err = receiver_open(&rcv, &txp, identity_g, "antani", &obj);
    TEST_ASSERT_TRUE(!err);
    while (!rcv.firmware_received && !err) {
        err = receiver_chunk(&rcv);
        if (err) {
            continue; // This should be applied everywhere
        }
        loop(&txp, 1000);
    }
    TEST_ASSERT_TRUE_MESSAGE(err == NETWORK_ERROR, err_as_str(err));
}

void test_get_firmware_invalid_size(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, PULL_UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    err = receiver_open(&rcv, &txp, identity_g, "firmware/invalid_size", &obj);
    TEST_ASSERT_TRUE(!err);
    while (!rcv.firmware_received && !err) {
        err = receiver_chunk(&rcv);
        if (err) {
            continue; // This should be applied everywhere
        }
        loop(&txp, 1000);
    }
    TEST_ASSERT_TRUE_MESSAGE(err == INVALID_SIZE_ERROR, err_as_str(err));
}
