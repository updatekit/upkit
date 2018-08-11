#include <libpull/common.h>
#include <libpull/pipeline/lzss.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "support/support.h"

#define LZSS_INPUT "../assets/slot_a.compressed"
#define LZSS_OUTPUT "../assets/slot_a.decompressed"
#define LZSS_EXPECTED "../assets/slot_a.pristine"

void ntest_prepare(void) {
    srand(time(NULL));
}

void ntest_clean(void) {}

 int test_save_init(pipeline_ctx_t* ctx, void* more) {
     static int fp;
     if ((fp = open(LZSS_OUTPUT, O_WRONLY | O_CREAT, 0666)) < 0) {
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

 pipeline_func_t test_save = {
     .init = test_save_init,
     .process = test_save_process
 };

/* This function must test the lzss decoding stage of the pipeline. */
void test_decode(void) {
    int readed;
    uint8_t buffer[100];
    pipeline_ctx_t ctx_save;
    pipeline_ctx_t ctx_lzss;

    // 0. Open the lzss encoded input file
    int input = open(LZSS_INPUT, O_RDONLY);
    nTEST_TRUE(input >= 0);
    // 1. Prepare the pipeline
    test_save.init(&ctx_save, NULL);

    ctx_lzss.next_func = &test_save;
    ctx_lzss.next_ctx = &ctx_save;

    lzss_pipeline.init(&ctx_lzss, NULL);
    
    int r = 0;
    do {
        // 2. Read N bytes from the file to a buffer (with N random between 50:100)
        r = (rand() % 50) + 50;
        readed = read(input, buffer, r);
        // 3. Pass the readed bytes to the stage until the file is finished
        lzss_pipeline.process(&ctx_lzss, buffer, readed);
    } while (readed == r);
    close(input);
    // 4. Compare the decompressed file with the original file
    nTEST_TRUE(file_compare(LZSS_OUTPUT, LZSS_EXPECTED) == 0);
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_decode);
    nTEST_END();
    nTEST_RETURN();
}

