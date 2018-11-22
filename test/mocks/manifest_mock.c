#include "manifest_mock.h"

manifest_mock_t manifest_mock;

/* All the getter returns an integer or a pointer 
 * I'm doing a cast to the return type. In this way
 * in the case of numbers it remains 0 and in case
 * of a pointer is equal to NULL */
#define IMPLEMENT_INVALID_GETTER(type, name) \
    type get_##name##_invalid(const manifest_t* mt) { \
        return (type) 0; \
    }
#define IMPLEMENT_INVALID_GETTER_MEMORY(type, name) \
    type get_##name##_invalid(const manifest_t* mt, uint8_t* size) { \
        *size = 0; \
        return NULL; \
    }

FOREACH_ITEM(IMPLEMENT_INVALID_GETTER);

#define RESTORE_VALID_GETTER(type, name) \
    manifest_mock.get_##name = get_##name##_impl;
#define RESTORE_VALID_GETTER_MEMORY(type, name) \
    manifest_mock.get_##name = get_##name##_impl;

void manifest_mock_restore() {
    FOREACH_ITEM(RESTORE_VALID_GETTER);
    manifest_mock.verify_signature = verify_signature_impl; 
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
appid_t get_appid(const manifest_t *mt) {
  return manifest_mock.get_appid(mt);
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

/* Valid functions */
pull_error verify_signature(manifest_t* mt, keystore_t keystore) {
    return manifest_mock.verify_signature(mt, keystore);
}
pull_error sign_manifest_vendor(manifest_t* mt, const uint8_t *private_key,
                    uint8_t* signature_buffer) {
    return manifest_mock.sign_manifest_vendor(mt, private_key, signature_buffer);
}
pull_error sign_manifest_server(manifest_t* mt, const uint8_t *private_key,
                    uint8_t* signature_buffer) {
    return manifest_mock.sign_manifest_server(mt, private_key, signature_buffer);
}

/* Invalid functions */
pull_error verify_signature_invalid(manifest_t* mt, keystore_t keystore) {
	return GENERIC_ERROR;
}

pull_error sign_manifest_vendor_invalid(manifest_t* mt, const uint8_t *private_key,
		uint8_t* signature_buffer) {
	return GENERIC_ERROR;
}
pull_error sign_manifest_server_invalid(manifest_t* mt, const uint8_t *private_key,
		uint8_t* signature_buffer) {
	return GENERIC_ERROR;
}
