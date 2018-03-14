#ifndef MANIFEST_MOCK_H_
#define MANIFEST_MOCK_H_

#include "common/error.h"
#include "memory/manifest.h"
#include "memory/simple_manifest.h"

#define DEFINE_GETTER(type, name, ...) \
	type get_##name##_impl(const manifest_t* mt, ##__VA_ARGS__);

FOREACH_ITEM(DEFINE_GETTER)


#define DEFINE_STRUCT_ELEM(type, name, ...) \
		type (*get_##name) (manifest_t*, ##__VA_ARGS__);

	typedef struct {
		FOREACH_ITEM(DEFINE_STRUCT_ELEM)
			pull_error (*verify_manifest_vendor) (manifest_t*, digest_func, const uint8_t*, const uint8_t*, ecc_curve);
		pull_error (*verify_manifest_server) (manifest_t*, digest_func, const uint8_t*, const uint8_t*, ecc_curve);
		pull_error (*sign_manifest_vendor) (manifest_t*, digest_func, const uint8_t *, uint8_t*, ecc_curve);
		pull_error (*sign_manifest_server) (manifest_t*, digest_func, const uint8_t*, uint8_t*, ecc_curve);
	} manifest_mock_t;

extern manifest_mock_t manifest_mock;

#define DEFINE_INVALID_GETTER(type, name, ...) \
	type get_##name##_invalid(const manifest_t* mt, ##__VA_ARGS__);

/* Standard Implementations */ 
pull_error verify_manifest_vendor_impl(manifest_t* mt, digest_func f, const uint8_t *pub_x,
		const uint8_t *pub_y, ecc_curve curve);
pull_error verify_manifest_server_impl(manifest_t* mt, digest_func f, const uint8_t *pub_x,
		const uint8_t *pub_y, ecc_curve curve);
pull_error sign_manifest_vendor_impl(manifest_t* mt, digest_func f, const uint8_t *private_key,
		uint8_t* signature_buffer, ecc_curve curve);
pull_error sign_manifest_server_impl(manifest_t* mt, digest_func f, const uint8_t *private_key,
		uint8_t* signature_buffer, ecc_curve curve);

/* Invalid Implementations */
pull_error verify_manifest_vendor_invalid(manifest_t* mt, digest_func f, const uint8_t *pub_x,
		const uint8_t *pub_y, ecc_curve curve);
pull_error verify_manifest_server_invalid(manifest_t* mt, digest_func f, const uint8_t *pub_x,
		const uint8_t *pub_y, ecc_curve curve);
pull_error sign_manifest_vendor_invalid(manifest_t* mt, digest_func f, const uint8_t *private_key,
		uint8_t* signature_buffer, ecc_curve curve);
pull_error sign_manifest_server_invalid(manifest_t* mt, digest_func f, const uint8_t *private_key,
		uint8_t* signature_buffer, ecc_curve curve);

#endif
