#include <stdio.h>
#include <string.h>

#include "memory.h"

int main(void) {
    UnityBegin(__FILENAME__);
    RUN_TEST(test_memory, test_memory_line);
    return UnityEnd();
}
