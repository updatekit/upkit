#include <stdio.h>
#include <string.h>

#include <zephyr.h>

#include "security.h"

int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}

int main(void) {
    nTEST_INIT();
    nTEST_RUN(test_security);
    nTEST_END();
    return 0;
}
