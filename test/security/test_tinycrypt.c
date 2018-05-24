#include "common/libpull.h"
#include "memory/memory_objects.h"
#include "security/digest.h"
#include "security/sha256.h"
#include "security/ecc.h"

#include "memory_mock.h" // Mock 
#include "memory_file_posix.h" // Real Implmentation

#include "manifest_mock.h" // Mock
#include "memory/simple_manifest.h" // Real Implementation

#include "test_runner.h"
#include "unity.h"
#include <string.h>

digest_func df;
ecc_func_t ef;
mem_object obj_1;

#include "test_verifier.h"

#define FOREACH_TEST(DO) \
    DO(ecc_verify, 0)\
    DO(ecc_verify_invalid_signature, 0)\
    DO(sha256, 0)\
    DO(sha256_invalid_init, 0)\
    DO(sha256_invalid_update, 0)\
    DO(sha256_invalid_final, 0)\
    DO(sign,0)\
    DO(verify_object_valid,0)\
    DO(verify_object_invalid_object,0)\
    DO(verify_object_invalid_read,0)\
    DO(verify_object_invalid_digest_init,0)\
    DO(verify_object_invalid_digest_update,0)\
    DO(verify_object_invalid_key,0)
TEST_RUNNER();

void setUp() {
    TEST_ASSERT_TRUE(memory_open(&obj_1, OBJ_1, WRITE_ALL) == PULL_SUCCESS);
    df = tinycrypt_digest_sha256;
    ef = tinycrypt_secp256r1_ecc;
    memory_mock_restore();
    manifest_mock_restore();
 }
