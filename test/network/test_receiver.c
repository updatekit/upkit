#include "unity.h"
#include "receiver.h"
#include "memory.h"
#include "metadata.h"
#include "simple_metadata.h"
#include "memory_objects.h"
#include "memory_file_posix.h"
#include "transport.h"
#include "async.h"
#include "error.h"
#include "async_libcoap.h"
#include "transport_libcoap.h"
#include "sample_data.h"

#define PROV_SERVER "localhost"

void setUp(void) {
}

void tearDown(void) {
    metadata invalid_mt;
    bzero(&invalid_mt, sizeof(metadata));
    mem_object obj_t;
    pull_error err = write_firmware_metadata(OBJ_2, &invalid_mt, &obj_t);
    TEST_ASSERT_TRUE(!err);
}

void test_get_firmware(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    mem_object obj_t;
    err = receiver_open(&rcv, &txp, "firmware", OBJ_2, &obj_t);
    printf("Starting the firmware reception\n");
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

void test_get_firmware_dtls(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    dtls_ecdh_data_t ecdh_data = {
        .curve = CURVE_SECP256R1,
        .priv_key = (uint8_t*) priv_g,
        .pub_key_x = (uint8_t*) x,
        .pub_key_y = (uint8_t*) y
    };
    pull_error err = txp_init(&txp, PROV_SERVER, 0, DTLS_ECDH, &ecdh_data);
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

/*
TODO: refactor the receiver to handler the received error.
In case of non recoverable errors it should abort the process.
For example in this case the ota resource is not available on the
server and thus the client should stop requisting for this resource
void test_get_firmware_invalid_resource(void) {
    txp_ctx txp;
    receiver_ctx rcv;
    pull_error err = txp_init(&txp, PROV_SERVER, 0, UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    mem_object obj_t;
    err = receiver_open(&rcv, &txp, "ota", OBJ_1, &obj_t);
    printf("Starting the firmware reception\n");
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
*/
