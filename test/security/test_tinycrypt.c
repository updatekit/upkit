#include <libpull/common.h>
#include <libpull/security.h>
#include <libpull/memory/memory_objects.h>
#include <libpull/memory/simple_manifest.h>
#include <string.h>
#include "platform_headers.h"

mem_object_t obj_a;
#define TEST_AES 1

#include "test_verifier.h"

void ntest_prepare() {
    nTEST_TRUE(memory_open(&obj_a, OBJ_A, WRITE_ALL) == PULL_SUCCESS);
}

void ntest_clean() {};

int main() {
    nTEST_INIT();
    nTEST_RUN(test_ecc_verify);
    nTEST_RUN(test_ecc_verify_invalid_signature);
    nTEST_RUN(test_sha256);
    nTEST_RUN(test_sign);
    nTEST_RUN(test_aes128);
    nTEST_RUN(test_verify_object_valid);
    nTEST_RUN(test_verify_object_invalid_key);
    nTEST_END();
    nTEST_RETURN();
}

