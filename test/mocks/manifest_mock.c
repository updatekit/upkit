#include "manifest_mock.h"

manifest_mock_t manifest_mock;

/* All the getter returns an integer or a pointer */
#define IMPLEMENT_INVALID_GETTER(type, name, ...) \
    type get_##name##_invalid(const manifest_t* mt, ##__VA_ARGS__) { \
        return NULL; \
    }

FOREACH_ITEM(IMPLEMENT_INVALID_GETTER);

#define RESTORE_VALID_GETTER(type, name, ...) \
    manifest_mock.get_##name = get_##name##_impl;

void manifest_mock_restore() {
    FOREACH_ITEM(RESTORE_VALID_GETTER);
    manifest_mock.verify_manifest_vendor = verify_manifest_vendor_impl; 
    manifest_mock.verify_manifest_server = verify_manifest_server_impl;
    manifest_mock.sign_manifest_vendor = sign_manifest_vendor_impl;
    manifest_mock.sign_manifest_server = sign_manifest_server_impl;
}

/* This does not needs to be mocked */
void print_manifest_impl(const manifest_t* mt);
void print_manifest(const manifest_t* mt) {
    return print_manifest_impl(mt);
}

version_t get_version(const manifest_t *mt) {
  return manifest_mock.get_version(mt);
}
platform_t get_platform(const manifest_t *mt) {
  return manifest_mock.get_platform(mt);
}
address_t get_size(const manifest_t *mt) {
    return manifest_mock.get_size(mt);
}
address_t get_offset(const manifest_t *mt) {
  return manifest_mock.get_offset(mt);
}
uint8_t *get_server_key_x(const manifest_t *mt) {
  return manifest_mock.get_server_key_x(mt);
}
uint8_t *get_server_key_y(const manifest_t *mt) {
  return manifest_mock.get_server_key_y(mt);
}
uint8_t *get_digest(const manifest_t *mt) {
    return manifest_mock.get_digest(mt);
}
uint8_t* get_vendor_signature_r(const manifest_t *mt, uint8_t* size) {
    return manifest_mock.get_vendor_signature_r(mt, size);
}
uint8_t* get_vendor_signature_s(const manifest_t *mt, uint8_t* size) {
    return manifest_mock.get_vendor_signature_s(mt, size);
}
uint8_t* get_server_signature_r(const manifest_t *mt, uint8_t* size) {
    return manifest_mock.get_server_signature_r(mt, size);
}
uint8_t* get_server_signature_s(const manifest_t *mt, uint8_t* size) {
    return manifest_mock.get_server_signature_s(mt, size);
}
pull_error verify_manifest_vendor(manifest_t* mt, digest_func f, const uint8_t *pub_x,
        const uint8_t *pub_y, ecc_curve curve) {
    return manifest_mock.verify_manifest_vendor(mt, f, pub_x, pub_y, curve);
}
pull_error verify_manifest_server(manifest_t* mt, digest_func f, const uint8_t *pub_x,
        const uint8_t *pub_y, ecc_curve curve) {
    return manifest_mock.verify_manifest_server(mt, f, pub_x, pub_y, curve);
}
pull_error sign_manifest_vendor(manifest_t* mt, digest_func f, const uint8_t *private_key,
                    uint8_t* signature_buffer, ecc_curve curve) {
    return manifest_mock.sign_manifest_vendor(mt, f, private_key, signature_buffer, curve);
}
pull_error sign_manifest_server(manifest_t* mt, digest_func f, const uint8_t *private_key,
                    uint8_t* signature_buffer, ecc_curve curve) {
    return manifest_mock.sign_manifest_server(mt, f, private_key, signature_buffer, curve);
}

/* Invalid functions */

pull_error verify_manifest_vendor_invalid(manifest_t* mt, digest_func f, const uint8_t *pub_x,
		const uint8_t *pub_y, ecc_curve curve) {
	return GENERIC_ERROR;
}
pull_error verify_manifest_server_invalid(manifest_t* mt, digest_func f, const uint8_t *pub_x,
		const uint8_t *pub_y, ecc_curve curve) {
	return GENERIC_ERROR;
}

pull_error sign_manifest_vendor_invalid(manifest_t* mt, digest_func f, const uint8_t *private_key,
		uint8_t* signature_buffer, ecc_curve curve) {
	return GENERIC_ERROR;
}
pull_error sign_manifest_server_invalid(manifest_t* mt, digest_func f, const uint8_t *private_key,
		uint8_t* signature_buffer, ecc_curve curve) {
	return GENERIC_ERROR;
}
