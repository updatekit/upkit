#include <libpull/common.h>
#include <libpull/security.h>
#include <libpull/memory.h>

#include "invalid_digest.h" // Test Support
#include "support/support.h"

#define BUFFER_LEN 1024
static uint8_t buffer[BUFFER_LEN];

void test_ecc_verify(void) {
    pull_error err;
    err = ecc_verify(vendor_x_g, vendor_y_g, vendor_r_g, vendor_s_g, hash_g, get_curve_size());
    nTEST_TRUE(err == PULL_SUCCESS);
}

void test_ecc_verify_invalid_signature(void) {
    pull_error err;
    err = ecc_verify(server_x_g, server_y_g, vendor_r_g, vendor_s_g, server_priv_g, get_curve_size());
    nTEST_TRUE(err == VERIFICATION_FAILED_ERROR);
}

void test_sha256(void) {
    digest_ctx ctx;
    pull_error err = digest_init(&ctx);
    nTEST_TRUE(!err);
    err = digest_update(&ctx, (void*) data_g, 128);
    nTEST_TRUE(!err);
    uint8_t* hash = digest_finalize(&ctx);
    nTEST_TRUE(hash != NULL);
    nTEST_COMPARE_MEM(hash_g, hash, 32);
}

void test_sign(void) {
    uint8_t signature[64];
    pull_error err = ecc_sign(server_priv_g, signature, hash_g, get_curve_size());
    nTEST_TRUE(!err);
    nTEST_TRUE(ecc_verify(server_x_g, server_y_g, signature, signature+32, hash_g, get_curve_size()) == PULL_SUCCESS);
}
void test_sign_invalid_hash_size(void) {
    uint8_t signature[64];
    pull_error err = ecc_sign(server_priv_g, signature, hash_g, 10);
    nTEST_TRUE(!err);
}

void test_verify_object_valid(void) {
    pull_error err = verify_object(&obj_a, safestore_g, buffer, BUFFER_LEN);
    nTEST_TRUE(err == PULL_SUCCESS, "%s\n", err_as_str(err));
}

void test_verify_object_invalid_key(void) {
    safestore_t safestore_invalid;
    memcpy(&safestore_invalid, &safestore_g, sizeof(safestore_t));
    safestore_invalid.keystore.x[0] = 0xab;
    pull_error err = verify_object(&obj_a, safestore_invalid, buffer, BUFFER_LEN);
    nTEST_TRUE(err, "%s\n", err_as_str(err));
}
