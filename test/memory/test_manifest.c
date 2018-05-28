#include <libpull/memory.h>
#include <libpull/security/sha256.h>

#include "tinycrypt_default_cspring.h" // Test Support
#include "invalid_digest.h" // Test Support
#include "sample_data.h" // Test Support
#include "memory_posix.h" // Posix file

#include "unity.h"
#include "test_runner.h"
#include <string.h>

#define FOREACH_TEST(DO) \
    DO(manifest_version, 0) \
    DO(manifest_platform, 0) \
    DO(manifest_size,0) \
    DO(manifest_offset,0) \
    DO(manifest_server_key_x,0) \
    DO(manifest_server_key_y,0) \
    DO(manifest_digest,0) \
    DO(manifest_vendor_signature_r,0) \
    DO(manifest_vendor_signature_s,0) \
    DO(manifest_server_signature_r,0) \
    DO(manifest_server_signature_s,0) \
    DO(sign_verify_manifest_vendor,0) \
    DO(sign_verify_manifest_server,0) \
    DO(sign_verify_manifest_vendor_invalid_digest,0)
TEST_RUNNER();

#define BUFFER_LEN 1024

/* Default parameters used for the test */
version_t version_g = 0xdead;
platform_t platform_g = 0xabcd;
address_t size_g = 0x1000;
address_t offset_g = 0x100; 
unsigned char server_key_x_g[] = "server_key_x";
unsigned char server_key_y_g[] = "server_key_y";
uint8_t* digest_g = (uint8_t*) hash_g;
uint8_t* vendor_signature_r_g = (uint8_t*) vendor_r_g;
uint8_t* vendor_signature_s_g = (uint8_t*) vendor_s_g;
uint8_t* server_signature_r_g = (uint8_t*) vendor_r_g;
uint8_t* server_signature_s_g = (uint8_t*) vendor_s_g;

/* setUp function called every time a new test is executed */
manifest_t mt;
uint8_t size = 32;
void setUp(void) {
    bzero(&mt, sizeof(manifest_t));
    set_size(&mt, size_g);
    set_offset(&mt, offset_g);
    set_digest(&mt, digest_g);
    set_platform(&mt, platform_g);
    set_version(&mt, version_g);
    set_server_key_x(&mt, server_key_x_g);
    set_server_key_y(&mt, server_key_y_g);
    set_vendor_signature_r(&mt, vendor_signature_r_g, size);
    set_vendor_signature_s(&mt, vendor_signature_s_g, size);
    set_server_signature_r(&mt, server_signature_r_g, size);
    set_server_signature_s(&mt, server_signature_s_g, size);
}

void test_manifest_version(void) {
    TEST_ASSERT_TRUE(version_g == get_version(&mt));
}
void test_manifest_platform(void) {
    TEST_ASSERT_TRUE(platform_g == get_platform(&mt));
}
void test_manifest_size(void) {
    TEST_ASSERT_TRUE(size_g == get_size(&mt));
}
void test_manifest_offset(void) {
    TEST_ASSERT_TRUE(offset_g == get_offset(&mt));
}
void test_manifest_server_key_x(void) {
    TEST_ASSERT_EQUAL_UINT8_ARRAY(server_key_x_g, get_server_key_x(&mt), 32);
}
void test_manifest_server_key_y(void) {
    TEST_ASSERT_EQUAL_UINT8_ARRAY(server_key_y_g, get_server_key_y(&mt), 32);
}
void test_manifest_digest(void) {
    TEST_ASSERT_EQUAL_UINT8_ARRAY(digest_g, get_digest(&mt), 32);
}
void test_manifest_vendor_signature_r(void) {
    uint8_t new_size;
    TEST_ASSERT_EQUAL_MEMORY(vendor_signature_r_g, get_vendor_signature_r(&mt, &new_size), size);
    TEST_ASSERT_EQUAL_UINT8(size, new_size);
}
void test_manifest_vendor_signature_s(void) {
    uint8_t new_size;
    TEST_ASSERT_EQUAL_MEMORY(vendor_signature_s_g, get_vendor_signature_s(&mt, &new_size), size);
    TEST_ASSERT_EQUAL_UINT8(size, new_size);
}
void test_manifest_server_signature_r(void) {
    uint8_t new_size;
    TEST_ASSERT_EQUAL_MEMORY(server_signature_r_g, get_server_signature_r(&mt, &new_size), size);
    TEST_ASSERT_EQUAL_UINT8(size, new_size);
}
void test_manifest_server_signature_s(void) {
    uint8_t new_size;
    TEST_ASSERT_EQUAL_MEMORY(server_signature_s_g, get_server_signature_s(&mt, &new_size), size);
    TEST_ASSERT_EQUAL_UINT8(size, new_size);
}

void test_sign_verify_manifest_vendor(void) {
    uint8_t buffer[BUFFER_LEN];
    pull_error err = sign_manifest_vendor(&mt, tinycrypt_digest_sha256, 
                        vendor_priv_g, buffer, tinycrypt_secp256r1_ecc);
    TEST_ASSERT_TRUE(err == PULL_SUCCESS);
    err = verify_manifest_vendor(&mt, tinycrypt_digest_sha256, vendor_x_g, vendor_y_g, tinycrypt_secp256r1_ecc);
}
void test_sign_verify_manifest_vendor_invalid_digest(void) {
    uint8_t buffer[BUFFER_LEN];
    digest_func f = tinycrypt_digest_sha256;
    f.init = invalid_init;
    pull_error err = sign_manifest_vendor(&mt, f, vendor_priv_g, buffer, tinycrypt_secp256r1_ecc);
    TEST_ASSERT_TRUE(err);
    err = verify_manifest_vendor(&mt, f, vendor_x_g, vendor_y_g, tinycrypt_secp256r1_ecc);
    TEST_ASSERT_TRUE(err);
}
void test_sign_verify_manifest_server(void) {
    uint8_t buffer[BUFFER_LEN];
    pull_error err = sign_manifest_server(&mt, tinycrypt_digest_sha256, vendor_priv_g, buffer, tinycrypt_secp256r1_ecc);
    TEST_ASSERT_TRUE(err == PULL_SUCCESS);
    err = verify_manifest_server(&mt, tinycrypt_digest_sha256, server_x_g, server_y_g, tinycrypt_secp256r1_ecc);
}
