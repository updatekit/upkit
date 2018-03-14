#include "common/error.h"
#include "security/digest.h"
#include "security/ecc.h"
#include "security/verifier.h"
#include "memory/memory_objects.h"
#include "memory/manifest.h"
#include "memory/memory.h"

#include "memory_mock.h"
#include "manifest_mock.h"

#include "invalid_digest.h" // Test Support
#include "sample_data.h" // Test Support

#define BUFFER_LEN 1024
static uint8_t buffer[BUFFER_LEN];

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
    pull_error err = func.init(&ctx);
    TEST_ASSERT_TRUE(!err);
    err = func.update(&ctx, (void*) data_g, 128);
    TEST_ASSERT_TRUE(!err);
    uint8_t* hash = func.finalize(&ctx);
    TEST_ASSERT_TRUE(hash != NULL);
    TEST_ASSERT_EQUAL_MEMORY(hash_g, hash, 32);
}

void test_sha256_invalid_init(void) {
    digest_func digest = func;
    pull_error err = digest.init(NULL);
    TEST_ASSERT_TRUE(err == DIGEST_INIT_ERROR);
}

void test_sha256_invalid_update(void) {
    digest_ctx ctx;
    digest_func digest = func;
    pull_error err = digest.init(&ctx);
    TEST_ASSERT_TRUE(!err);
    err = digest.update(NULL, (void*) data_g, 128);
    TEST_ASSERT_TRUE(err = DIGEST_UPDATE_ERROR);
    err = digest.update(&ctx, NULL, 128);
    TEST_ASSERT_TRUE(err = DIGEST_UPDATE_ERROR);
    err = digest.update(&ctx, (void*) data_g, 0);
    TEST_ASSERT_TRUE(err = DIGEST_UPDATE_ERROR);
}

void test_sha256_invalid_final(void) {
    digest_ctx ctx;
    digest_func digest = func;
    pull_error err = digest.init(&ctx);
    TEST_ASSERT_TRUE(!err);
    err = digest.update(&ctx, (void*) data_g, 128);
    TEST_ASSERT_TRUE(!err);
    uint8_t* hash = digest.finalize(NULL);
    TEST_ASSERT_TRUE(hash == NULL);
}

void test_sign(void) {
    uint8_t signature[64];
    pull_error err = ecc_sign(priv_g, signature, hash_g, 32, secp256r1);
    TEST_ASSERT_TRUE(!err);
    TEST_ASSERT_TRUE(ecc_verify(x, y, signature, signature+32, hash_g, 32, secp256r1) == PULL_SUCCESS);
}
void test_sign_invalid_hash_size(void) {
    uint8_t signature[64];
    pull_error err = ecc_sign(priv_g, signature, hash_g, 10, secp256r1);
    TEST_ASSERT_TRUE(!err);
}

void test_verify_object_valid(void) {
    mem_object obj_t;
    pull_error err = verify_object(OBJ_1, func, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE_MESSAGE(err == PULL_SUCCESS, err_as_str(err));
}

void test_verify_object_invalid_object(void) {
    mem_object obj_t;
    pull_error err;
    err = verify_object(42, func, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE(err);
}

void test_verify_object_invalid_read(void) {
    memory_mock.memory_read_impl = memory_read_invalid;

    mem_object obj_t;
    pull_error err;
    err = verify_object(OBJ_1, func, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE(err == MEMORY_READ_ERROR);
}

void test_verify_object_invalid_digest_init(void) {
    mem_object obj_t;
    pull_error err;
    func.init = invalid_init;
    err = verify_object(OBJ_1, func, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE_MESSAGE(err == DIGEST_INIT_ERROR, err_as_str(err));
}

void test_verify_object_invalid_digest_update(void) {
    mem_object obj_t;
    pull_error err;
    func.update = invalid_update;
    err = verify_object(OBJ_1, func, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE_MESSAGE(err == DIGEST_UPDATE_ERROR, err_as_str(err));
}

void test_verify_object_invalid_key(void) {
    mem_object obj_t;
    pull_error err = verify_object(OBJ_1, func, y, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE_MESSAGE(err == VERIFICATION_FAILED_ERROR, err_as_str(err));
}
