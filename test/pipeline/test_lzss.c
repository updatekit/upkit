#include <libpull/common.h>
#include <libpull/pipeline/lzss.h>
#include <string.h>

#include "support/support.h"

void ntest_prepare(void) {}
void ntest_clean(void) {}

/* This function must test the lzss decoding stage of the pipeline. */
void test_decode(void) {
    // 0. Open the lzss encoded input file
    // 1. Read N bytes from the file to a buffer (with N random between 50:100)
    // 2. Pass the readed bytes to the stage until the file is finished
    // 3. Compare the decompressed file with the original file
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_decode);
    nTEST_END();
    nTEST_RETURN();
}

