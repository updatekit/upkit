#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libpull/common.h>
#include <libpull/pipeline/bspatch.h>
#include "support/support.h"

#define BSDIFF_INPUT1 "../assets/slot_a.bin"
#define BSDIFF_INPUT2 "../assets/slot_b.bin"
#define BSDIFF_PATCH "../assets/slot.patch"
#define BSDIFF_PATCHED "../assets/slot_b.patched"

void ntest_prepare(void) {
    srand(time(NULL));
}

void ntest_clean(void) {}

 int test_save_init(pipeline_ctx_t* ctx, void* more) {
     static int fp;
     if ((fp = open(BSDIFF_PATCHED, O_WRONLY | O_CREAT, 0666)) < 0) {
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

/* This function tests the bspatch stage of the pipeline. */
void test_patch(void) {
    char buffer[100];
    // 0. Open the original file;
    int input1 = open(BSDIFF_INPUT1, O_RDONLY);
    nTEST_TRUE(input1 >= 0);
    nTEST_TRUE(lseek(input1, 0L, SEEK_END) > 0);
    long bufsize = lseek(input1, 0, SEEK_CUR);
    nTEST_TRUE(lseek(input1, 0L, SEEK_SET) == 0);
    char* buffer1 = malloc(sizeof(char)*bufsize);
    nTEST_TRUE(read(input1, buffer1, bufsize) == bufsize);
    // 1. Open the patch file.
    int patch = open(BSDIFF_PATCH, O_RDONLY);
    nTEST_TRUE(patch >= 0);
    // 3. Prepare the pipeline.
    pipeline_ctx_t bspatch_ctx;
    pipeline_ctx_t save_ctx;
    
    pipeline_bspatch.init(&bspatch_ctx, NULL);
    test_save.init(&save_ctx, NULL);

    bspatch_ctx.next_func = &test_save;
    bspatch_ctx.next_ctx = &save_ctx;
    bspatch_ctx.stage_data = buffer1;
    // 4. Start the algorithm.
    int readed = 0, r = 0;
    do {
        r = (random() % 50) + 50;
        // 4.1. Read N bytes from the original file.
        readed = read(patch, &buffer, r);
        // 4.2. Pass the readed bytes to the bspatch stage of the pipeline.
        nTEST_TRUE(pipeline_bspatch.process(&bspatch_ctx, (uint8_t*)buffer, readed) >= 0);
    } while (readed == r);
    // 5. Compare the decompressed file with the original file.
    nTEST_TRUE(file_compare(BSDIFF_INPUT2, BSDIFF_PATCHED) == 0);
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_patch);
    nTEST_END();
    nTEST_RETURN();
}

