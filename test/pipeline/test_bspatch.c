#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/pipeline/bspatch.h>
#include "support/support.h"

#define BSDIFF_INPUT1 "../assets/slot_a.bin"
#define BSDIFF_INPUT2 "../assets/slot_b.bin"
#define BSDIFF_PATCH "../assets/slot.patch"
#define BSDIFF_PATCHED "../assets/slot_b.patched"

void ntest_prepare(void) {
    srand(time(NULL));
    restore_assets();
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

/* This function tests the bspatch stage of the pipeline. */
void test_patch(void) {
    char buffer[100];
    // 0. Open the original file;
    mem_object_t obj_a;
    nTEST_TRUE(memory_open(&obj_a, OBJ_A, READ_ONLY) == PULL_SUCCESS);

    // 1. Open the patch file.
    int patch = open(BSDIFF_PATCH, O_RDONLY);
    nTEST_TRUE(patch >= 0);
    // 3. Prepare the pipeline.
    pipeline_ctx_t bspatch_ctx;
    pipeline_ctx_t save_ctx;
    
    pipeline_bspatch_init(&bspatch_ctx, NULL);
    test_save_init(&save_ctx, NULL);

    bspatch_ctx.next_stage = &test_save_process;
    bspatch_ctx.next_ctx = &save_ctx;

    bspatch_ctx.stage_data = &obj_a;
    // 4. Start the algorithm.
    int readed = 0, r = 0;
    do {
        r = (random() % 50) + 50;
        // 4.1. Read N bytes from the original file.
        readed = read(patch, &buffer, r);
        // 4.2. Pass the readed bytes to the bspatch stage of the pipeline.
        nTEST_TRUE(bspatch_pipeline(&bspatch_ctx, (uint8_t*)buffer, readed) >= 0);
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

