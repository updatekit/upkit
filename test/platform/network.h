#ifndef TEST_PLATFORM_NETWORK_H_
#define TEST_PLATFORM_NETWORK_H_

#include <libpull/network.h>
#include <libpull/common.h>

#include "stest.h"
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

static txp_ctx txp;

void stest_prepare() {
    cb_called = 0;
    pull_error err = txp_init(&txp, SERVER_ADDR, SERVER_PORT, PULL_UDP, NULL);
    STEST_TRUE(!err)
}

void stest_clean() {
    txp_end(&txp);
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
        txp.loop = 0;
    }
}

STEST_DEFINE(test_txp) {
    uint16_t version = EXPECTED_VERSION;
    pull_error err = txp_on_data(&txp, handler_cmp_memory, &version);
    STEST_TRUE(!err);
    while (cb_called < 10) {
        err = txp_request(&txp, GET, "/version", NULL, 0);
        STEST_TRUE(!err);
        loop_once(&txp, TIMEOUT_MS);
    }
    STEST_TRUE(cb_called == 10);
}

STEST_DEFINE(test_receiver) {
    mem_object_t obj;
    pull_error err = memory_open(&obj, OBJ_2, WRITE_ALL);
    STEST_TRUE(!err);
    receiver_ctx rcv;
    err = receiver_open(&rcv, &txp, identity_g, "/firmware", &obj);
    STEST_TRUE(!err);
    while (!rcv.firmware_received) {
        err = receiver_chunk(&rcv);
        STEST_TRUE(!err);
        loop(&txp, TIMEOUT_MS);
    }
    err = receiver_close(&rcv);
    STEST_TRUE(!err);
}

#endif /* TEST_PLATFORM_NETWORK_H_ */
