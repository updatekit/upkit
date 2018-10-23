#include <libpull/common.h>
#include <libpull/security.h>
#include <libpull/memory/memory_objects.h>
#include <string.h>

#include "memory_mock.h" // Mock
#include "platform_headers.h"

#include "manifest_mock.h" // Mock
#include <libpull/memory/simple_manifest.h> // Real Implementation

digest_func df;
ecc_func_t ef;
mem_object_t obj_a;

#include "test_verifier.h"

void ntest_prepare() {
    nTEST_TRUE(memory_open(&obj_a, OBJ_A, WRITE_ALL) == PULL_SUCCESS);
    df = tinydtls_digest_sha256;
    ef = tinydtls_secp256r1_ecc;
    memory_mock_restore();
    manifest_mock_restore();
}

void ntest_clean() {};

int main() {
    nTEST_INIT();
    nTEST_RUN(test_ecc_verify);
    nTEST_RUN(test_ecc_verify_invalid_signature);
    nTEST_RUN(test_sha256);
    nTEST_RUN(test_sha256_invalid_init);
    nTEST_RUN(test_sha256_invalid_update);
    nTEST_RUN(test_sha256_invalid_final);
    nTEST_RUN(test_verify_object_valid);
    nTEST_RUN(test_verify_object_invalid_read);
    nTEST_RUN(test_verify_object_invalid_digest_init);
    nTEST_RUN(test_verify_object_invalid_digest_update);
    nTEST_RUN(test_verify_object_invalid_key);
    //nTEST_RUN(test_sign);
    nTEST_END();
    nTEST_RETURN();
}

