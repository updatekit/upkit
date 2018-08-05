#include <libpull/common.h>
#include <libpull/pipeline/lzss.h>
#include <string.h>

#include "support/support.h"

void ntest_prepare(void) {}
void ntest_clean(void) {}

void test_decode(void) {

}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_decode);
    nTEST_END();
    nTEST_RETURN();
}

