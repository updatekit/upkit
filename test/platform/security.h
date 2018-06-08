#ifndef TEST_PLATFORM_SECURITY_H_
#define TEST_PLATFORM_SECURITY_H_

#include <libpull/security.h>
#include "shared.h"

static digest_func df;
static ecc_func_t ef;

void specialize_crypto_functions() {
#if WITH_TINYDTLS
    df = tinydtls_digest_sha256;
    ef = tinydtls_secp256r1_ecc;
#elif WITH_TINYCRYPT
    df = tinycrypt_digest_sha256;
    ef = tinycrypt_secp256r1_ecc;
#endif
}

void setUp(void) {
    specialize_crypto_functions();
}

void tearDown(void) {}

DEFINE_TEST(test_digest_functions) {
    TEST_ASSERT_NOT_NULL(df.init);
    TEST_ASSERT_NOT_NULL(df.init);
    TEST_ASSERT_NOT_NULL(df.finalize);
}

DEFINE_TEST(test_ecc_functions) {
    TEST_ASSERT_NOT_NULL(ef.verify);
}

DEFINE_TEST(test_security) {
    uint8_t* hash;
    digest_ctx ctx;
    pull_error err = df.init(&ctx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(err, PULL_SUCCESS, err_as_str(err));
    err = df.update(&ctx, (void*) data_g, 128);
    TEST_ASSERT_EQUAL_INT_MESSAGE(err, PULL_SUCCESS, err_as_str(err));
    hash = df.finalize(&ctx);
    TEST_ASSERT_EQUAL_INT_MESSAGE(err, PULL_SUCCESS, err_as_str(err));
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(hash_g, hash, 32, "Invalid result hash\n");
    err = ef.verify(vendor_x_g, vendor_y_g, vendor_r_g, vendor_s_g, hash_g, ef.curve_size);
    TEST_ASSERT_EQUAL_INT_MESSAGE(err, PULL_SUCCESS, err_as_str(err));
}

#endif /* TEST_PLATFORM_SECURITY_H_ */
