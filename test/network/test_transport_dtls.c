#include "common/error.h"
#include "network/transport.h"
#include "transport_libcoap.h"
#include "memory/manifest.h"
#include "memory/simple_manifest.h"

#include "tinydtls.h"
#include <coap/coap.h>

#include "sample_data.h"
#include "test_runner.h"
#include "unity.h"

#define FOREACH_TEST(DO) \
    DO(udp, 0) \
    DO(dtls_ecdsa, 0)
TEST_RUNNER();

#define PROV_SERVER "localhost"

void get(void);
void get_blockwise(void);
void echo(void);
void stress_echo(void);

#define LOOP_CONDITION(condition) { \
    int max_retry = max_retry_g; \
    while (max_retry-- && condition) { \
        int ret = coap_run_once(ctx.coap_ctx, timeout_g); \
        TEST_ASSERT_TRUE(ret >= 0); \
        if (ret < timeout_g) { \
            max_retry = max_retry_g; \
        } \
    }}

txp_ctx ctx;
int max_retry_g;
int timeout_g;
int cb_called;

void setUp(void) {
    cb_called = 0;
    max_retry_g = 10;
    timeout_g = 1000;
}

void tearDown(void) {
    txp_end(&ctx);
}

void test_udp(void) {
    // Set up udp connection
    pull_error error = txp_init(&ctx, PROV_SERVER, 0, CONN_UDP, NULL);
    TEST_ASSERT_TRUE(!error);
    get();
    echo();
    stress_echo();
    get_blockwise();
}

void test_dtls_ecdsa(void) {
    // Set up dtls ecdh connection
    dtls_ecdh_data_t ecdh_data = {
        .curve = CURVE_SECP256R1,
        .priv_key = (uint8_t *) priv_g,
        .pub_key_x = (uint8_t *) x,
        .pub_key_y = (uint8_t *) y
    };
    pull_error error = txp_init(&ctx, PROV_SERVER, 0, CONN_DTLS_ECDH, &ecdh_data);
    TEST_ASSERT_TRUE(!error);
    get();
    echo();
    stress_echo();
    get_blockwise();
}


/* Handler callbacks */
void handler_cmp_memory(pull_error err, const char* data, const int len, void* more) {
    TEST_ASSERT_TRUE(!err);
    TEST_ASSERT_EQUAL_MEMORY(more, data, len);
    cb_called++;
}

void handler_stress_echo(pull_error err, const char* data, const int len, void* more) {
    TEST_ASSERT_TRUE(!err);
    TEST_ASSERT_TRUE(len == sizeof(int));
    int counter = *(int*)data;
    TEST_ASSERT_EQUAL_INT((*(int*)more)++, counter++);
    pull_error error = txp_request(&ctx, GET, "echo", (const char*) &counter, sizeof(int));
    TEST_ASSERT_TRUE(!error);
    cb_called++;
}

void handler_blockwise(pull_error err, const char* data, const int len, void* more) {
    TEST_ASSERT_TRUE(!err);
    TEST_ASSERT_TRUE(len>0 && data != NULL);
    cb_called++;
}

/* Unit tests */
void echo(void) {
    cb_called = 0;
    char data[] = "expected";
    pull_error error = txp_on_data(&ctx, handler_cmp_memory, data);
    TEST_ASSERT_TRUE(!error);
    error = txp_request(&ctx, GET, "echo", data, strlen(data));
    TEST_ASSERT_TRUE(!error);
    LOOP_CONDITION(!cb_called);
    TEST_ASSERT_EQUAL_INT(1, cb_called);
}

void stress_echo(void) {
    cb_called = 0;
    int counter = 0;
    pull_error error = txp_on_data(&ctx, handler_stress_echo, &counter);
    TEST_ASSERT_TRUE(!error);
    error = txp_request(&ctx, GET, "echo", (const char*) &counter, sizeof(int));
    TEST_ASSERT_TRUE(!error);
    LOOP_CONDITION(cb_called < 100);
    TEST_ASSERT_EQUAL_INT(100, counter);
    TEST_ASSERT_EQUAL_INT(100, cb_called);
}

void get(void) {
    cb_called = 0;
    uint16_t expected = 0xdead;
    pull_error error = txp_on_data(&ctx, handler_cmp_memory, &expected);
    TEST_ASSERT_TRUE(!error);
    error = txp_request(&ctx, GET, "version", NULL, 0);
    TEST_ASSERT_TRUE(!error);
    LOOP_CONDITION(!cb_called);
    expected = 0x0;
    error = txp_on_data(&ctx, handler_cmp_memory, &expected);
    TEST_ASSERT_TRUE(!error);
    error = txp_request(&ctx, GET, "version/invalid", NULL, 0);
    TEST_ASSERT_TRUE(!error);
    LOOP_CONDITION(cb_called < 2);
    TEST_ASSERT_EQUAL_INT(2, cb_called);
}

void get_blockwise(void) {
    cb_called = 0;
    max_retry_g = 15;
    pull_error error = txp_on_data(&ctx, handler_blockwise, NULL);
    TEST_ASSERT_TRUE(!error);
    error = txp_request(&ctx, GET_BLOCKWISE2, "firmware", NULL, 0);
    TEST_ASSERT_TRUE(!error);
    LOOP_CONDITION(cb_called < 20);
    TEST_ASSERT_EQUAL_INT(20, cb_called);
}
