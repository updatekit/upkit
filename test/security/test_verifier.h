#include <libpull/common.h>
#include <libpull/security.h>
#include <libpull/memory.h>

#include "memory_mock.h"
#include "manifest_mock.h"

#include "invalid_digest.h" // Test Support
#include "support/support.h"

#define BUFFER_LEN 1024
static uint8_t buffer[BUFFER_LEN];

void test_ecc_verify(void) {
    pull_error err;
    err = ef.verify(vendor_x_g, vendor_y_g, vendor_r_g, vendor_s_g, hash_g, ef.curve_size);
    nTEST_TRUE(err == PULL_SUCCESS);
}

void test_ecc_verify_invalid_signature(void) {
    pull_error err;
    err = ef.verify(server_x_g, server_y_g, vendor_r_g, vendor_s_g, server_priv_g, ef.curve_size);
    nTEST_TRUE(err == VERIFICATION_FAILED_ERROR);
}

void test_sha256(void) {
    digest_ctx ctx;
    pull_error err = df.init(&ctx);
    nTEST_TRUE(!err);
    err = df.update(&ctx, (void*) data_g, 128);
    nTEST_TRUE(!err);
    uint8_t* hash = df.finalize(&ctx);
    nTEST_TRUE(hash != NULL);
    nTEST_COMPARE_MEM(hash_g, hash, 32);
}

void test_sha256_invalid_init(void) {
    digest_func digest = df;
    pull_error err = digest.init(NULL);
    nTEST_TRUE(err == DIGEST_INIT_ERROR);
}

void test_sha256_invalid_update(void) {
    digest_ctx ctx;
    digest_func digest = df;
    pull_error err = digest.init(&ctx);
    nTEST_TRUE(!err);
    err = digest.update(NULL, (void*) data_g, 128);
    nTEST_TRUE(err = DIGEST_UPDATE_ERROR);
    err = digest.update(&ctx, NULL, 128);
    nTEST_TRUE(err = DIGEST_UPDATE_ERROR);
    err = digest.update(&ctx, (void*) data_g, 0);
    nTEST_TRUE(err = DIGEST_UPDATE_ERROR);
}

void test_sha256_invalid_final(void) {
    digest_ctx ctx;
    digest_func digest = df;
    pull_error err = digest.init(&ctx);
    nTEST_TRUE(!err);
    err = digest.update(&ctx, (void*) data_g, 128);
    nTEST_TRUE(!err);
    uint8_t* hash = digest.finalize(NULL);
    nTEST_TRUE(hash == NULL);
}

void test_sign(void) {
    uint8_t signature[64];
    pull_error err = ef.sign(server_priv_g, signature, hash_g, ef.curve_size);
    nTEST_TRUE(!err);
    nTEST_TRUE(ef.verify(server_x_g, server_y_g, signature, signature+32, hash_g, ef.curve_size) == PULL_SUCCESS);
}
void test_sign_invalid_hash_size(void) {
    uint8_t signature[64];
    pull_error err = ef.sign(server_priv_g, signature, hash_g, 10);
    nTEST_TRUE(!err);
}

void test_verify_object_valid(void) {
    pull_error err = verify_object(&obj_a, df, vendor_x_g, vendor_y_g, ef, buffer, BUFFER_LEN);
    nTEST_TRUE(err == PULL_SUCCESS, "%s\n", err_as_str(err));
}

void test_verify_object_invalid_read(void) {
    memory_mock.memory_read_impl = memory_read_invalid;

    pull_error err;
    err = verify_object(&obj_a, df, vendor_x_g, vendor_y_g, ef, buffer, BUFFER_LEN);
    nTEST_TRUE(MEMORY_READ_ERROR == err, "%s\n", err_as_str(err));
}

void test_verify_object_invalid_digest_init(void) {
    pull_error err;
    df.init = invalid_init;
    err = verify_object(&obj_a, df, vendor_x_g, vendor_y_g, ef, buffer, BUFFER_LEN);
    nTEST_TRUE(err == DIGEST_INIT_ERROR, "%s\n", err_as_str(err));
}

void test_verify_object_invalid_digest_update(void) {
    pull_error err;
    df.update = invalid_update;
    err = verify_object(&obj_a, df, vendor_x_g, vendor_y_g, ef, buffer, BUFFER_LEN);
    nTEST_TRUE(err, "%s\n", err_as_str(err));
}

void test_verify_object_invalid_key(void) {
    pull_error err = verify_object(&obj_a, df, vendor_y_g, vendor_y_g, ef, buffer, BUFFER_LEN);
    nTEST_TRUE(err, "%s\n", err_as_str(err));
}
