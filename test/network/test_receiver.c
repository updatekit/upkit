#include "unity.h"
#include "receiver.h"
#include "memory.h"
#include "manifest.h"
#include "simple_manifest_impl.h"
#include "memory_objects.h"
#include "memory_file_posix.h"
#include "transport.h"
#include "async.h"
#include "error.h"
#include "async_libcoap.h"
#include "tinydtls.h"
#include "transport_libcoap.h"
#include "sample_data.h"

#define PROV_SERVER "localhost"

void setUp(void) {
}

void tearDown(void) {
    manifest_t invalid_mt;
    bzero(&invalid_mt, sizeof(manifest_t));
    mem_object obj_t;
    pull_error err = write_firmware_manifest(OBJ_2, &invalid_mt, &obj_t);
    TEST_ASSERT_TRUE(!err);
}

void test_get_firmware(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, CONN_UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    mem_object obj_t;
    err = receiver_open(&rcv, &txp, "firmware", OBJ_2, &obj_t);
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
        .curve = CURVE_SECP256R1,
        .priv_key = (uint8_t*) priv_g,
        .pub_key_x = (uint8_t*) x,
        .pub_key_y = (uint8_t*) y
    };
    pull_error err = txp_init(&txp, PROV_SERVER, 0, CONN_DTLS_ECDH, &ecdh_data);
    TEST_ASSERT_TRUE(!err);
    mem_object obj_t;
    err = receiver_open(&rcv, &txp, "firmware", OBJ_2, &obj_t);
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

void test_receiver_open_invalid_id(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, CONN_UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    mem_object obj_t;
    err = receiver_open(&rcv, &txp, "firmware", 42, &obj_t);
    TEST_ASSERT_TRUE(err == RECEIVER_OPEN_ERROR);
}

void test_receiver_chunk_invalid_transport(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, CONN_UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    mem_object obj_t;
    err = receiver_open(&rcv, &txp, "firmware", OBJ_2, &obj_t);
    TEST_ASSERT_TRUE(!err);
    rcv.txp = NULL;
    err = receiver_chunk(&rcv);
    TEST_ASSERT_TRUE(err);
}

void test_receiver_close_invalid_memory(void) {
    receiver_ctx rcv;
    rcv.obj = NULL;
    pull_error err = receiver_close(&rcv);
    TEST_ASSERT_TRUE(err == RECEIVER_CLOSE_ERROR);
}

void test_get_firmware_invalid_resource(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, CONN_UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    mem_object obj_t;
    err = receiver_open(&rcv, &txp, "antani", OBJ_2, &obj_t);
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

/*
void test_get_firmware_invalid_size(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, CONN_UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    mem_object obj_t;
    err = receiver_open(&rcv, &txp, "firmware/invalid_size", OBJ_2, &obj_t);
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
*/
