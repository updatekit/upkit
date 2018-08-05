#include <libpull/common.h>
#include <libpull/network/subscriber.h>
#include <libpull/network/async_interface.h>
#include <libpull/memory/memory_objects.h>

#include "support/support.h"

#define PROV_SERVER "localhost"

void ntest_prepare(void) {}

void ntest_clean(void) {
    restore_assets();
}

static void check_update_cb(pull_error err, const char* data, int len, void* more) {
    subscriber_ctx* ctx = (subscriber_ctx*) more;
    nTEST_TRUE(data != NULL);
    nTEST_TRUE(len == 2);
    version_t version;
    memcpy(&version, data, sizeof(version_t));
    nTEST_COMPARE_HEX(0xD, version);
    break_loop(ctx->conn);
}

void test_update_polling(void) {
    subscriber_ctx ctx;
    conn_ctx conn;
    conn_init(&conn, PROV_SERVER, 0, PULL_UDP, NULL);
    mem_object_t obj_t;
    pull_error err = subscribe(&ctx, &conn, "version", &obj_t);
    nTEST_TRUE(!err);
    err = check_updates(&ctx, check_update_cb);
    nTEST_TRUE(!err);
    loop(&conn, 1000);
    // XXX This is not valid.. The loop function should return the reason why
    // it returned
    unsubscribe(&ctx);
}

// TODO: add more tests making requests to invalid endpoints

int main() {
    nTEST_INIT();
    nTEST_RUN(test_update_polling);
    nTEST_END();
    nTEST_RETURN();
}


