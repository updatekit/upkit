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
    pull_error (*verify_signature) (manifest_t*, keystore_t keystore);
    pull_error (*sign_manifest_vendor) (manifest_t*, const uint8_t *, uint8_t*);
    pull_error (*sign_manifest_server) (manifest_t*, const uint8_t*, uint8_t*);
} manifest_mock_t;

extern manifest_mock_t manifest_mock;
void manifest_mock_restore();

#define DEFINE_INVALID_GETTER(type, name) \
	type get_##name##_invalid(const manifest_t* mt);
#define DEFINE_INVALID_GETTER_MEMORY(type, name) \
	type get_##name##_invalid(const manifest_t* mt, uint8_t* size);

/* Standard Implementations */ 
pull_error verify_signature_impl(manifest_t* mt, keystore_t keystore);
pull_error sign_manifest_vendor_impl(manifest_t* mt, const uint8_t *private_key,
		uint8_t* signature_buffer);
pull_error sign_manifest_server_impl(manifest_t* mt, const uint8_t *private_key,
		uint8_t* signature_buffer);

/* Invalid Implementations */
pull_error verify_signature(manifest_t* mt, keystore_t keystore);
pull_error sign_manifest_vendor_invalid(manifest_t* mt, const uint8_t *private_key,
		uint8_t* signature_buffer);
pull_error sign_manifest_server_invalid(manifest_t* mt, const uint8_t *private_key,
		uint8_t* signature_buffer);

#endif
