#include <libpull/common.h>
#include <libpull/pipeline/buffer.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "support/support.h"

#define BUFFER_INPUT    "../assets/slot_a.bin"
#define BUFFER_OUTPUT   "../assets/slot_a.buffered"
#define BUFFER_EXPECTED "../assets/slot_a.pristine"

void ntest_prepare(void) {
    srand(time(NULL));
}

void ntest_clean(void) {}

int test_save_init(pipeline_ctx_t* ctx, void* more) {
    static int fp;
    if ((fp = open(BUFFER_OUTPUT, O_WRONLY | O_CREAT, 0666)) < 0) {
        printf("Error opening output file\n");
        return -1;
    }
    ctx->stage_data = &fp;
    return 0;
}

int test_save_process(pipeline_ctx_t* ctx, uint8_t* buf, int l) {
    int *fp = (int*) ctx->stage_data;
    if (write(*fp, buf, l) != l) {
        printf("Error writing bytes\n");
        return -1;
    }
    return l;
}

int test_save_clear(pipeline_ctx_t* ctx) {
    int *fp = (int*) ctx->stage_data;
    close(fp);
    return 0; 
}

pipeline_func_t test_save = {
    .init = test_save_init,
    .process = test_save_process,
    .clear = test_save_clear
};


/* The goals of the buffer tests are two:
 * - check that the output file is equal to the input file;
 * - check if the stage behaves as expected: this means
 *   it passes the bytes to the next stage only when the
 *   buffer is full. */
void test_single_buffer(void) {
    int readed;
    uint8_t buffer[100];
    pipeline_ctx_t ctx_save;
    pipeline_ctx_t ctx_buffer;

    // 0. Open the input file
    int input = open(BUFFER_INPUT, O_RDONLY);
    nTEST_TRUE(input >= 0);

    // 1. Prepare the pipeline
    test_save.init(&ctx_save, NULL);

    ctx_buffer.next_func = &test_save;
    ctx_buffer.next_ctx = &ctx_save;

    buffer_pipeline.init(&ctx_buffer, NULL);

    int r = 0;
    do {
        // 2. Read N bytes from the file to a buffer (with N random between 50:100)
        r = (rand() % 50) + 50;
        readed = read(input, buffer, r);
        // 3. Pass the readed bytes to the stage until the file is finished
        buffer_pipeline.process(&ctx_buffer, buffer, readed);
    } while (readed == r);
    buffer_pipeline.clear(&ctx_buffer);
    close(input);
    // 4. Compare the decompressed file with the original file
    nTEST_TRUE(file_compare(BUFFER_OUTPUT, BUFFER_EXPECTED) == 0);
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_single_buffer);
    nTEST_END();
    nTEST_RETURN();
}

