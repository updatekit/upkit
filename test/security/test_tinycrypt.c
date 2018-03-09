#include "unity.h"
#include "mock_memory.h"
#include "mock_metadata.h"
#include "sample_data.h"
#include "tinycrypt.h"
#include "tinydtls.h"
#include "digest.h"
#include "sha256.h"
#include "ecc.h"
#include "verifier.h"
#include "error.h"
#include "simple_metadata.h"
#include "memory_objects.h"
#include "memory_file_posix.h"
#include "test_verifier.h"
#include <string.h>

DIGEST_FUNC(tinycrypt);

int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}

void test_ecc_verify(void) {
    pull_error err;
    err = ecc_verify(x, y, r, s, hash_g, 32, secp256r1);
    TEST_ASSERT_TRUE(err == PULL_SUCCESS);
}

void test_ecc_verify_invalid_curve(void) {
    pull_error err;
    err = ecc_verify(x, y, r, s, hash_g, 32, secp224r1);
    TEST_ASSERT_TRUE(err == NOT_SUPPORTED_CURVE_ERROR);
}

void test_ecc_verify_invalid_signature(void) {
    pull_error err;
    err = ecc_verify(x, y, r, s, priv_g, 32, secp256r1);
    TEST_ASSERT_TRUE(err == VERIFICATION_FAILED_ERROR);
}

void test_sha256(void) {
    digest_ctx ctx;
    digest_func digest = tinycrypt_digest_sha256;
    pull_error err = digest.init(&ctx);
    TEST_ASSERT_TRUE(!err);
    err = digest.update(&ctx, (void*) data_g, 128);
    TEST_ASSERT_TRUE(!err);
    uint8_t* hash = digest.finalize(&ctx);
    TEST_ASSERT_TRUE(hash != NULL);
    TEST_ASSERT_EQUAL_MEMORY(hash_g, hash, 32);
}

void test_sha256_invalid_init(void) {
    digest_func digest = tinycrypt_digest_sha256;
    pull_error err = digest.init(NULL);
    TEST_ASSERT_TRUE(err == SHA256_INIT_ERROR);
}

void test_sha256_invalid_update(void) {
    digest_ctx ctx;
    digest_func digest = tinycrypt_digest_sha256;
    pull_error err = digest.init(&ctx);
    TEST_ASSERT_TRUE(!err);
    err = digest.update(NULL, (void*) data_g, 128);
    TEST_ASSERT_TRUE(SHA256_UPDATE_ERROR);
    err = digest.update(&ctx, NULL, 128);
    TEST_ASSERT_TRUE(SHA256_UPDATE_ERROR);
    err = digest.update(&ctx, (void*) data_g, 0);
    TEST_ASSERT_TRUE(SHA256_UPDATE_ERROR);
}

void test_sha256_invalid_final(void) {
    digest_ctx ctx;
    digest_func digest = tinycrypt_digest_sha256;
    pull_error err = digest.init(&ctx);
    TEST_ASSERT_TRUE(!err);
    err = digest.update(&ctx, (void*) data_g, 128);
    TEST_ASSERT_TRUE(!err);
    uint8_t* hash = digest.finalize(NULL);
    TEST_ASSERT_TRUE(hash == NULL);
}

void test_verifier(void) {
    test_verify_all(tinycrypt_digest_sha256);
}
