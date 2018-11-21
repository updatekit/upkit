#include <libpull/memory.h>
#include <string.h>

#include "support/support.h"

#define BUFFER_LEN 1024

/* Default parameters used for the test */
version_t version_g = 0xdead;
platform_t platform_g = 0xabcd;
address_t size_g = 0x1000;
address_t offset_g = 0x100; 
uint8_t* digest_g = (uint8_t*) hash_g;
uint8_t* vendor_signature_r_g = (uint8_t*) vendor_r_g;
uint8_t* vendor_signature_s_g = (uint8_t*) vendor_s_g;
uint8_t* server_signature_r_g = (uint8_t*) vendor_r_g;
uint8_t* server_signature_s_g = (uint8_t*) vendor_s_g;

/* setUp function called every time a new test is executed */
manifest_t mt;
uint8_t size = 32;
void ntest_prepare(void) {
    bzero(&mt, sizeof(manifest_t));
    set_size(&mt, size_g);
    set_offset(&mt, offset_g);
    set_digest(&mt, digest_g);
    set_version(&mt, version_g);
    set_server_key_x(&mt, (uint8_t*) server_x_g);
    set_server_key_y(&mt, (uint8_t*) server_y_g);
    set_vendor_signature_r(&mt, vendor_signature_r_g, size);
    set_vendor_signature_s(&mt, vendor_signature_s_g, size);
    set_server_signature_r(&mt, server_signature_r_g, size);
    set_server_signature_s(&mt, server_signature_s_g, size);
}

void ntest_clean() {};

void test_manifest_version(void) {
    nTEST_TRUE(version_g == get_version(&mt));
}
void test_manifest_size(void) {
    nTEST_TRUE(size_g == get_size(&mt));
}
void test_manifest_offset(void) {
    nTEST_TRUE(offset_g == get_offset(&mt));
}
void test_manifest_server_key_x(void) {
    nTEST_COMPARE_MEM(server_x_g, get_server_key_x(&mt), 32*sizeof(uint8_t));
}
void test_manifest_server_key_y(void) {
    nTEST_COMPARE_MEM(server_y_g, get_server_key_y(&mt), 32*sizeof(uint8_t));
}
void test_manifest_digest(void) {
    nTEST_COMPARE_MEM(digest_g, get_digest(&mt), 32*sizeof(uint8_t));
}
void test_manifest_vendor_signature_r(void) {
    uint8_t new_size;
    nTEST_COMPARE_MEM(vendor_signature_r_g, get_vendor_signature_r(&mt, &new_size), size);
    nTEST_COMPARE_UINT(size, new_size);
}
void test_manifest_vendor_signature_s(void) {
    uint8_t new_size;
    nTEST_COMPARE_MEM(vendor_signature_s_g, get_vendor_signature_s(&mt, &new_size), size);
    nTEST_COMPARE_UINT(size, new_size);
}
void test_manifest_server_signature_r(void) {
    uint8_t new_size;
    nTEST_COMPARE_MEM(server_signature_r_g, get_server_signature_r(&mt, &new_size), size);
    nTEST_COMPARE_UINT(size, new_size);
}
void test_manifest_server_signature_s(void) {
    uint8_t new_size;
    nTEST_COMPARE_MEM(server_signature_s_g, get_server_signature_s(&mt, &new_size), size);
    nTEST_COMPARE_UINT(size, new_size);
}

void test_sign_verify_manifest(void) {
    uint8_t buffer[BUFFER_LEN];
    pull_error err = sign_manifest_vendor(&mt, vendor_priv_g, buffer);
    nTEST_TRUE(err == PULL_SUCCESS);
    err = sign_manifest_server(&mt, server_priv_g, buffer);
    nTEST_TRUE(err == PULL_SUCCESS);
    err = verify_signature(&mt, keystore_g);
    nTEST_TRUE(err == PULL_SUCCESS);
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_manifest_version);
    nTEST_RUN(test_manifest_size);
    nTEST_RUN(test_manifest_offset);
    nTEST_RUN(test_manifest_server_key_x);
    nTEST_RUN(test_manifest_server_key_y);
    nTEST_RUN(test_manifest_digest);
    nTEST_RUN(test_manifest_vendor_signature_r);
    nTEST_RUN(test_manifest_vendor_signature_s);
    nTEST_RUN(test_manifest_server_signature_r);
    nTEST_RUN(test_manifest_server_signature_s);
    nTEST_RUN(test_sign_verify_manifest);
    nTEST_END();
    nTEST_RETURN();
}

