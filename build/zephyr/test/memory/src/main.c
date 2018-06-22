#include <stdio.h>
#include <string.h>

#include "memory.h"

int main(void) {
    nTEST_INIT();
    nTEST_RUN(test_memory);
    nTEST_END();
    return 0;
}
