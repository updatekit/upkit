#include <libpull/common.h>
#include <libpull/pipeline.h>
#include <string.h>

#include "support/support.h"

void ntest_prepare(void) {}
void ntest_clean(void) {}

/* The idea of the pipeline is to have
 * a set of modules that takes N bytes in input and pass
 * to the next stage N times 1 byte. In this way we can
 * slightly simplify each stage and thus reduce the size.
 * This test should evaluate the pipeline concept stressing
 * it using random data and a random number of bytes at each
 * stage.
 */

#define VAL_TYPE uint8_t
int success;

int test_val_init(pipeline_ctx_t* ctx, void* more) {
    ctx->more = more;
    return 0;
}

int test_val_process(pipeline_ctx_t* ctx, uint8_t* buf, int l) {
    int i = 0;
    for (i=0; i<l; i++) {
        if (buf[i] != *((VAL_TYPE*)ctx->more)) {
            success = 0;
        }
    }
    return l;
}

pipeline_func_t test_val = {
    .init = test_val_init,
    .process = test_val_process
};

int test_sum_init(pipeline_ctx_t* ctx, void* more) {
    /* This funtion does nothing */
    return 0;
}

/* This testing pipeline function should process the data by adding
 * the value one to each received byte. In this way we can test, at
 * the end of N pipeline stages if the result is N. */
int test_sum_process(pipeline_ctx_t* ctx, uint8_t* buf, int l) {
    int i;
    for (i=0; i<l; i++) {
        buf[i] += 1;
        ctx->next_func->process(ctx->next_ctx, &buf[i], 1);
    }
    return l;
}

pipeline_func_t test_sum = {
    .init = test_sum_init,
    .process = test_sum_process
};

/** 
 * \brief This test must pass a buffer to a stage 
 */
void test_one_stage(void) {
    success = 1;
    uint8_t buffer[100];
    uint8_t expected = 1;
    int i = 0;
    for (i=0; i<100; i++) {
        buffer[i] = 0;
    }
    pipeline_ctx_t ctx_sum;
    pipeline_ctx_t ctx_val;

    test_val.init(&ctx_val, &expected);
    test_sum.init(&ctx_sum, NULL);

    ctx_sum.next_func = &test_val;
    ctx_sum.next_ctx = &ctx_val;

    test_sum.process(&ctx_sum, buffer, 100);
    nTEST_TRUE(success);
}

void test_ten_stages(void) {
    success = 1;
    uint8_t buffer[100];
    uint8_t expected = 10;
    int i = 0;
    pipeline_ctx_t ctx_sum[10];
    pipeline_ctx_t ctx_val;
    for (i=0; i<100; i++) {
        buffer[i] = 0;

    }
    for (i=0; i<10; i++) {
        test_sum.init(&ctx_sum[i], NULL);
    }
    for (i=0; i<9; i++) {
        ctx_sum[i].next_func = &test_sum;
        ctx_sum[i].next_ctx = &ctx_sum[i+1];
    }

    test_val.init(&ctx_val, &expected);

    ctx_sum[9].next_func = &test_val;
    ctx_sum[9].next_ctx = &ctx_val;

    test_sum.process((pipeline_ctx_t*) &ctx_sum, buffer, 100);
    nTEST_TRUE(success);
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_one_stage);
    nTEST_RUN(test_ten_stages);
    nTEST_END();
    nTEST_RETURN();
}

