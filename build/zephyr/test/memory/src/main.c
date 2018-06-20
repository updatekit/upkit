#include <stdio.h>
#include <string.h>

#include "memory.h"

int main(void) {
    UnityBegin("memory");
    RUN_TEST(test_memory, test_memory_line);
    UnityEnd();
    while(1){};
    return 0;
}
