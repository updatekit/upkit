#include "unity.h"
#include "mock_memory.h"
#include "mock_manifest.h"
#include "ecc.h"
#include "sample_data.h"
#include "tinydtls.h"
#include "digest.h"
#include "sha256.h"
#include "memory_objects.h"
#include "error.h"
#include "test_verifier.h"
#include "verifier.h"
#include "simple_manifest_impl.h"
#include "memory_file_posix.h"

DIGEST_FUNC(tinydtls);

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
    TEST_ASSERT_TRUE(err ==  VERIFICATION_FAILED_ERROR);
}

void test_sha256(void) {
    digest_ctx ctx;
    digest_func digest = tinydtls_digest_sha256;
    digest.init(&ctx);
    digest.update(&ctx, (void*) data_g, 128);
    uint8_t* hash = digest.finalize(&ctx);
    TEST_ASSERT_TRUE(hash != NULL);
    TEST_ASSERT_EQUAL_MEMORY(hash_g, hash, 32);
}

#define DEFINE_TEST(name) \
    void test_verify_object_##name (void) {\
        verify_object_##name (tinydtls_digest_sha256); \
    }

FOREACH_TEST(DEFINE_TEST)
