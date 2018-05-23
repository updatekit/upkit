#include "common/libpull.h"
#include "network/subscriber.h"
#include "network/async.h"
#include "memory/memory_objects.h"

#include "memory_file_posix.h"
#include "transport_libcoap.h"

#include "test_runner.h"
#include "unity.h"

#define FOREACH_TEST(DO) \
    DO(update_polling,0)

TEST_RUNNER();

#define PROV_SERVER "localhost"

void setUp(void) {
}

void tearDown(void) {
}

static void check_update_cb(pull_error err, const char* data, int len, void* more) {
    subscriber_ctx* ctx = (subscriber_ctx*) more;
    TEST_ASSERT(data != NULL);
    TEST_ASSERT_TRUE(len == 2);
    version_t version;
    memcpy(&version, data, sizeof(version_t));
    TEST_ASSERT_EQUAL_HEX16(0xdead, version);
    break_loop(ctx->txp);
}

void test_update_polling(void) {
    subscriber_ctx ctx;
    txp_ctx txp;
    txp_init(&txp, PROV_SERVER, 0, CONN_UDP, NULL);
    mem_object obj_t;
    pull_error err = subscribe(&ctx, &txp, "version", &obj_t);
    TEST_ASSERT_TRUE(!err);
    err = check_updates(&ctx, check_update_cb);
    TEST_ASSERT_TRUE(!err);
    loop(&txp, 1000);
    // XXX This is not valid.. The loop function should return the reason why
    // it returned
    unsubscribe(&ctx);
}

// TODO: add more tests making requests to invalid endpoints
