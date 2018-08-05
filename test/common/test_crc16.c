#include <libpull/common.h>
#include <libpull/memory.h>

#include "support/support.h"

void ntest_prepare(void) {}

void ntest_clean(void) {}

const char test_str[] = "this_string_is_a_test_for_the_CRC_16_function";
const uint16_t test_result = 0x99A3; // CRC-CCITT

void test_crc16(void) {
    uint16_t given;
    given = crc16(test_str, sizeof(test_str)-1);
    nTEST_COMPARE_HEX(given, test_result);
}

void test_crc16_failure(void) {
    uint16_t given;
    given = crc16(test_str, strlen(test_str)-2);
    nTEST_FALSE(given == test_result);
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_crc16);
    nTEST_RUN(test_crc16_failure);
    nTEST_END();
    nTEST_RETURN();
}
