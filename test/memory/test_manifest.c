#include "unity.h"
#include "common/error.h"
#include "memory/manifest.h"
#include "memory/memory.h"
#include "memory/simple_manifest.h"
#include "simple_manifest_impl.h"
#include "memory_file_posix.h"
#include "invalid_digest.h"
#include "sample_data.h"
#include "tinycrypt.h"
#include "tinycrypt_default_cspring.h"
#include "digest.h"
#include "sha256.h"
#include <string.h>

#define BUFFER_LEN 1024
DIGEST_FUNC(tinycrypt);

/* Default parameters used for the test */
version_t version_g = 0xdead;
platform_t platform_g = 0xabcd;
address_t size_g = 0x1000;
address_t offset_g = 0x100; 
unsigned char server_key_x_g[] = "server_key_x";
unsigned char server_key_y_g[] = "server_key_y";
uint8_t* digest_g = (uint8_t*) hash_g;
uint8_t* vendor_signature_r_g = (uint8_t*) r;
uint8_t* vendor_signature_s_g = (uint8_t*) s;
uint8_t* server_signature_r_g = (uint8_t*) r;
uint8_t* server_signature_s_g = (uint8_t*)s;

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
    set_vendor_signature_r(&mt, vendor_signature_r_g, &size);
    set_vendor_signature_s(&mt, vendor_signature_s_g, &size);
    set_server_signature_r(&mt, server_signature_r_g, &size);
    set_server_signature_s(&mt, server_signature_s_g, &size);
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
    TEST_ASSERT_EQUAL_MEMORY(vendor_signature_r_g, get_vendor_signature_r(&mt, &new_size), 32);
    TEST_ASSERT_EQUAL_UINT8(size, new_size);
}
void test_manifest_vendor_signature_s(void) {
    uint8_t new_size;
    TEST_ASSERT_EQUAL_MEMORY(vendor_signature_s_g, get_vendor_signature_s(&mt, &new_size), 32);
    TEST_ASSERT_EQUAL_UINT8(size, new_size);
}
void test_manifest_server_signature_r(void) {
    uint8_t new_size;
    TEST_ASSERT_EQUAL_MEMORY(server_signature_r_g, get_server_signature_r(&mt, &new_size), 32);
    TEST_ASSERT_EQUAL_UINT8(size, new_size);
}
void test_manifest_server_signature_s(void) {
    uint8_t new_size;
    TEST_ASSERT_EQUAL_MEMORY(server_signature_s_g, get_server_signature_s(&mt, &new_size), 32);
    TEST_ASSERT_EQUAL_UINT8(size, new_size);
}

void test_sign_verify_manifest_vendor(void) {
    uint8_t buffer[BUFFER_LEN];
    pull_error err = sign_manifest_vendor(&mt, tinycrypt_digest_sha256, priv_g, buffer, secp256r1);
    TEST_ASSERT_TRUE(err == PULL_SUCCESS);
    err = verify_manifest_vendor(&mt, tinycrypt_digest_sha256, x, y, secp256r1);
}
void test_sign_verify_manifest_vendor_invalid_digest(void) {
    uint8_t buffer[BUFFER_LEN];
    digest_func f = tinycrypt_digest_sha256;
    f.init = invalid_init;
    pull_error err = sign_manifest_vendor(&mt, f, priv_g, buffer, secp256r1);
    TEST_ASSERT_TRUE(err);
    err = verify_manifest_vendor(&mt, f, x, y, secp256r1);
    TEST_ASSERT_TRUE(err);
}
void test_sign_verify_manifest_server(void) {
    uint8_t buffer[BUFFER_LEN];
    pull_error err = sign_manifest_server(&mt, tinycrypt_digest_sha256, priv_g, buffer, secp256r1);
    TEST_ASSERT_TRUE(err == PULL_SUCCESS);
    err = verify_manifest_server(&mt, tinycrypt_digest_sha256, x, y, secp256r1);
}
