#include <libpull/common.h>
#include <libpull/network/receiver.h>
#include <libpull/memory/memory_objects.h>
#include <libpull/memory/manifest.h>

#include "support/support.h"

#define PROV_SERVER "localhost"

#define NO_MOCK(func) func##_StubWithCallback(func##_impl)

void tearDown(void) {
    NO_MOCK(memory_open);
    NO_MOCK(memory_flush);
    NO_MOCK(memory_write);
    NO_MOCK(memory_close);

    manifest_t invalid_mt;
    bzero(&invalid_mt, sizeof(manifest_t));
    mem_object obj_t;
    pull_error err = write_firmware_manifest(OBJ_A, &invalid_mt, &obj_t);
    TEST_ASSERT_TRUE(!err);
}

void test_get_firmware_invalid_memory_write(void) {
    NO_MOCK(memory_open);
    NO_MOCK(memory_flush);
    NO_MOCK(memory_close);

    conn_ctx conn;
    receiver_ctx rcv;
    pull_error err = conn_init(&conn, PROV_SERVER, 0, CONN_UDP, NULL);
    TEST_ASSERT_TRUE(!err);
    mem_object obj_t;
    err = receiver_open(&rcv, &conn, "firmware", OBJ_A, &obj_t);
    TEST_ASSERT_TRUE(!err);
    while (!rcv.firmware_received && !err) {
        err = receiver_chunk(&rcv);
        loop(&conn, 1000);
    }
    TEST_ASSERT_TRUE_MESSAGE(err == MEMORY_WRITE_ERROR, err_as_str(err));
}
