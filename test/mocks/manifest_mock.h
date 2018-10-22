#ifndef MANIFEST_MOCK_H_
#define MANIFEST_MOCK_H_

#include <libpull/common/error.h>
#include <libpull/memory/manifest.h>
#include <libpull/memory/simple_manifest.h>

#define DEFINE_GETTER(type, name) \
	type get_##name##_impl(const manifest_t* mt);

#define DEFINE_GETTER_MEMORY(type, name) \
	type get_##name##_impl(const manifest_t* mt, uint8_t* size);

FOREACH_ITEM(DEFINE_GETTER)

#define DEFINE_STRUCT_ELEM(type, name) \
		type (*get_##name) (const manifest_t*);
#define DEFINE_STRUCT_ELEM_MEMORY(type, name) \
		type (*get_##name) (const manifest_t*, uint8_t* size);

typedef struct {
    FOREACH_ITEM(DEFINE_STRUCT_ELEM);
    identity_t (*get_identity) (const manifest_t* mt);
    void (*set_identity) (manifest_t* mt, identity_t identity);
    pull_error (*verify_signature) (manifest_t*, digest_func, const uint8_t*, const uint8_t*, ecc_func_t);
    pull_error (*sign_manifest_vendor) (manifest_t*, digest_func, const uint8_t *, uint8_t*, ecc_func_t);
    pull_error (*sign_manifest_server) (manifest_t*, digest_func, const uint8_t*, uint8_t*, ecc_func_t);
} manifest_mock_t;

extern manifest_mock_t manifest_mock;
void manifest_mock_restore();

#define DEFINE_INVALID_GETTER(type, name) \
	type get_##name##_invalid(const manifest_t* mt);
#define DEFINE_INVALID_GETTER_MEMORY(type, name) \
	type get_##name##_invalid(const manifest_t* mt, uint8_t* size);

/* Standard Implementations */ 
identity_t get_identity_impl(const manifest_t* mt);
void set_identity_impl(manifest_t* mt, identity_t identity);

pull_error verify_signature_impl(manifest_t* mt, digest_func f, const uint8_t *pub_x,
		const uint8_t *pub_y, ecc_func_t ef);
pull_error sign_manifest_vendor_impl(manifest_t* mt, digest_func f, const uint8_t *private_key,
		uint8_t* signature_buffer, ecc_func_t ef);
pull_error sign_manifest_server_impl(manifest_t* mt, digest_func f, const uint8_t *private_key,
		uint8_t* signature_buffer, ecc_func_t ef);

/* Invalid Implementations */
identity_t get_identity_invalid(const manifest_t* mt);
void set_identity_invalid(manifest_t* mt, identity_t identity);

pull_error verify_signature(manifest_t* mt, digest_func f, const uint8_t *pub_x,
		const uint8_t *pub_y, ecc_func_t ef);
pull_error sign_manifest_vendor_invalid(manifest_t* mt, digest_func f, const uint8_t *private_key,
		uint8_t* signature_buffer, ecc_func_t ef);
pull_error sign_manifest_server_invalid(manifest_t* mt, digest_func f, const uint8_t *private_key,
		uint8_t* signature_buffer, ecc_func_t ef);

#endif
