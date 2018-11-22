#include <libpull/common.h>
#include <libpull/network.h>
#include <libpull/memory/memory_objects.h>

#include "support/support.h"

#define PROV_SERVER "localhost"

bool success = false;

void ntest_prepare(void) {
    success = false;
}

void ntest_clean(void) {
    restore_assets();
}

static void test_callback(pull_error err, const char* data, int len, void* more) {
    request_ctx_t* ctx = (request_ctx_t*) more;
    nTEST_TRUE(data != NULL);
    nTEST_TRUE(len == sizeof(version_t));
    version_t version;
    memcpy(&version, data, sizeof(version_t));
    nTEST_COMPARE_HEX(0xD, version);
    success=true;
    break_loop(ctx->conn);
}

void test_update_polling(void) {
    conn_ctx conn;
    pull_error err;
    request_ctx_t ctx;

    err = conn_init(&conn, PROV_SERVER, 0, PULL_UDP, NULL);
    nTEST_TRUE(!err);

    err = conn_on_data(&conn, test_callback, &ctx);
    nTEST_TRUE(!err);

    err = conn_request(&conn, GET, "version", NULL, 0);
    nTEST_TRUE(!err);
    loop(&conn, 1000);
    nTEST_TRUE(success);
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_update_polling);
    nTEST_END();
    nTEST_RETURN();
}


