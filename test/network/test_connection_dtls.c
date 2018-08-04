#include <libpull/common.h>
#include <libpull/network.h>
#include <libpull/memory.h>

#include "support/support.h"

#define PROV_SERVER "localhost"

void get(void);
void get_blockwise(void);
void echo(void);
void stress_echo(void);

#define LOOP_CONDITION(condition) { \
    int max_retry = max_retry_g; \
    while (max_retry-- && condition) { \
        int ret = coap_run_once(ctx.coap_ctx, timeout_g); \
        nTEST_TRUE(ret >= 0); \
        if (ret < timeout_g) { \
            max_retry = max_retry_g; \
        } \
    }}

txp_ctx ctx;
int max_retry_g;
int timeout_g;
int cb_called;

void ntest_prepare(void) {
    cb_called = 0;
    max_retry_g = 10;
    timeout_g = 1000;
}

void ntest_clean(void) {
    txp_end(&ctx);
}

void test_udp(void) {
    // Set up udp connection
    pull_error error = txp_init(&ctx, PROV_SERVER, 0, PULL_UDP, NULL);
    nTEST_TRUE(!error);
    get();
    echo();
    stress_echo();
}

void test_dtls_ecdsa(void) {
    // Set up dtls ecdh connection
    dtls_ecdh_data_t ecdh_data = {
        .priv_key = (uint8_t *) dtls_client_priv_g,
        .pub_key_x = (uint8_t *) dtls_client_x_g,
        .pub_key_y = (uint8_t *) dtls_client_y_g
    };
    pull_error error = txp_init(&ctx, PROV_SERVER, 0, PULL_DTLS_ECDH, &ecdh_data);
    nTEST_TRUE(!error);
    get();
    echo();
    stress_echo();
}


/* Handler callbacks */
void handler_cmp_memory(pull_error err, const char* data, const int len, void* more) {
    nTEST_TRUE(!err);
    nTEST_COMPARE_MEM(more, data, len);
    cb_called++;
}

void handler_stress_echo(pull_error err, const char* data, const int len, void* more) {
    nTEST_TRUE(!err);
    nTEST_TRUE(len == sizeof(int));
    int counter = *(int*)data;
    nTEST_COMPARE_INT((*(int*)more)++, counter++);
    pull_error error = txp_request(&ctx, GET, "echo", (const char*) &counter, sizeof(int));
    nTEST_TRUE(!error);
    cb_called++;
}

void handler_blockwise(pull_error err, const char* data, const int len, void* more) {
    nTEST_TRUE(!err);
    nTEST_TRUE(len>0 && data != NULL);
    cb_called++;
}

/* Unit tests */
void echo(void) {
    cb_called = 0;
    char data[] = "expected";
    pull_error error = txp_on_data(&ctx, handler_cmp_memory, data);
    nTEST_TRUE(!error);
    error = txp_request(&ctx, GET, "echo", data, strlen(data));
    nTEST_TRUE(!error);
    LOOP_CONDITION(!cb_called);
    nTEST_COMPARE_INT(1, cb_called);
}

void stress_echo(void) {
    cb_called = 0;
    int counter = 0;
    pull_error error = txp_on_data(&ctx, handler_stress_echo, &counter);
    nTEST_TRUE(!error);
    error = txp_request(&ctx, GET, "echo", (const char*) &counter, sizeof(int));
    nTEST_TRUE(!error);
    LOOP_CONDITION(cb_called < 100);
    nTEST_COMPARE_INT(100, counter);
    nTEST_COMPARE_INT(100, cb_called);
}

void get(void) {
    cb_called = 0;
    uint16_t expected = 0xd;
    pull_error error = txp_on_data(&ctx, handler_cmp_memory, &expected);
    nTEST_TRUE(!error);
    error = txp_request(&ctx, GET, "version", NULL, 0);
    nTEST_TRUE(!error);
    LOOP_CONDITION(!cb_called);
    expected = 0x0;
    error = txp_on_data(&ctx, handler_cmp_memory, &expected);
    nTEST_TRUE(!error);
    error = txp_request(&ctx, GET, "version/invalid", NULL, 0);
    nTEST_TRUE(!error);
    LOOP_CONDITION(cb_called < 2);
    nTEST_COMPARE_INT(2, cb_called);
}

int main() {
    nTEST_RUN(test_udp);
    nTEST_RUN(test_dtls_ecdsa);
}
