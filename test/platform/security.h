#ifndef TEST_PLATFORM_SECURITY_H_
#define TEST_PLATFORM_SECURITY_H_

#include <libpull/security.h>
#include "default_keys.h"
#include "ntest.h"

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

void ntest_prepare(void) {
    specialize_crypto_functions();
}

void ntest_clean(void) {}

void test_digest_functions(void) {
    nTEST_NOT_NULL(df.init);
    nTEST_NOT_NULL(df.init);
    nTEST_NOT_NULL(df.finalize);
}

void test_ecc_functions(void) {
    nTEST_NOT_NULL(ef.verify);
}

void test_security(void) {
    uint8_t* hash;
    digest_ctx ctx;
    pull_error err = df.init(&ctx);
    nTEST_COMPARE_INT(err, PULL_SUCCESS, err_as_str(err));
    err = df.update(&ctx, (void*) data_g, 128);
    nTEST_COMPARE_INT(err, PULL_SUCCESS, err_as_str(err));
    hash = df.finalize(&ctx);
    nTEST_COMPARE_INT(err, PULL_SUCCESS, err_as_str(err));
    nTEST_COMPARE_MEM(hash_g, hash, 32, "Invalid result hash\n");
    err = ef.verify(vendor_x_g, vendor_y_g, vendor_r_g, vendor_s_g, hash_g, ef.curve_size);
    nTEST_COMPARE_INT(err, PULL_SUCCESS, err_as_str(err));
}

#endif /* TEST_PLATFORM_SECURITY_H_ */
