#include <stdio.h>
#include <string.h>

#include <zephyr.h>

#include "security.h"

int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_digest_functions, test_digest_functions_line);
    RUN_TEST(test_ecc_functions, test_ecc_functions_line);
    RUN_TEST(test_security, test_security_line);
    UNITY_END();
    while(1){};
    return 0;
}
