#ifndef TEST_PLATFORM_NETWORK_H_
#define TEST_PLATFORM_NETWORK_H_

#include <libpull/network.h>
#include <libpull/common.h>

#include "ntest.h"
#include "platform_headers.h"
#include "default_keys.h"

#ifndef SERVER_ADDR
#define SERVER_ADDR "fd00:dead:beef::1"
#endif /* SERVER_ADDR */

#ifndef SERVER_PORT
#define SERVER_PORT 0
#endif /* SERVER_PORT */

#ifndef TIMEOUT_MS
#define TIMEOUT_MS 4000
#endif /* TIMEOUT_MS */

#ifndef EXPECTED_VERSION
#define EXPECTED_VERSION 0xD
#endif /* EXPECTED_VERSION */

static mem_object_t obj;
int cb_called = 0;

static conn_ctx conn;

void ntest_prepare() {
    cb_called = 0;
    pull_error err = conn_init(&conn, SERVER_ADDR, SERVER_PORT, PULL_UDP, NULL);
    nTEST_TRUE(!err, "Error initializing connection: %s", err_as_str(err));
}

void ntest_clean() {
    conn_end(&conn);
}

static void handler_cmp_memory(pull_error err, const char* data, const int len, void* more) {
    if (err) {
        log_error(err,"Error in the callback\n");
        return;
    }
    if (len != sizeof(version_t)) {
        printf("The received size is not valid\n");
        return;
    }
    if (memcmp(more, data, len) != 0) {
        printf("The data received is different from the expected\n");
        return;
    }
    cb_called++;
    if (cb_called >= 10) {
        conn.loop = 0;
    }
}

void test_conn(void) {
    uint16_t version = EXPECTED_VERSION;
    pull_error err = conn_on_data(&conn, handler_cmp_memory, &version);
    nTEST_TRUE(!err, "Error setting on data: %s", err_as_str(err));
    while (cb_called < 10) {
        err = conn_request(&conn, GET, "/version", NULL, 0);
        nTEST_TRUE(!err, "Error in conn_request: %s", err_as_str(err));
        loop_once(&conn, TIMEOUT_MS);
    }
    nTEST_COMPARE_INT(cb_called, 10);
}

void test_receiver(void) {
    mem_object_t obj;
    pull_error err = memory_open(&obj, OBJ_2, WRITE_ALL);
    nTEST_TRUE(!err, "Error opening memory: %s", err_as_str(err));
    receiver_ctx rcv;
    err = receiver_open(&rcv, &conn, identity_g, "/firmware", &obj);
    nTEST_TRUE(!err, "Error opening receiver: %s", err_as_str(err));
    while (!rcv.firmware_received) {
        err = receiver_chunk(&rcv);
        nTEST_TRUE(!err, "Error in receiver_chunk: %s", err_as_str(err));
        loop(&conn, TIMEOUT_MS);
    }
    err = receiver_close(&rcv);
    nTEST_TRUE(!err, "Error closing receiver: %s", err_as_str(err));
}

#endif /* TEST_PLATFORM_NETWORK_H_ */
