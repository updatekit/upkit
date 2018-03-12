#ifndef SIMPLE_MANIFEST_IMPL_H_
#define SIMPLE_MANIFEST_IMPL_H_

#include "memory/manifest.h"

#ifdef SIMPLE_MANIFEST

#define DEFINE_GETTER(type, name, ...) \
    type get_##name##_impl(const manifest_t* mt, ##__VA_ARGS__);

#define DEFINE_SETTER(type, name, ...) \
    void set_##name##_impl(manifest_t* mt, type name, ##__VA_ARGS__);

FOREACH_ITEM(DEFINE_GETTER)
FOREACH_ITEM(DEFINE_SETTER)

#undef DEFINE_GETTER
#undef DEFINE_SETTER

void print_manifest_impl(const manifest_t* mt);

pull_error verify_manifest_vendor_impl(manifest_t* mt, digest_func f, const uint8_t *pub_x,
        const uint8_t *pub_y, ecc_curve curve);
pull_error verify_manifest_server_impl(manifest_t* mt, digest_func f, const uint8_t *pub_x,
        const uint8_t *pub_y, ecc_curve curve);

pull_error sign_manifest_vendor_impl(manifest_t* mt, digest_func f, const uint8_t *private_key,
        uint8_t* signature_buffer, ecc_curve curve);
pull_error sign_manifest_server_impl(manifest_t* mt, digest_func f, const uint8_t *private_key,
        uint8_t* signature_buffer, ecc_curve curve);

#endif /* SIMPLE_MANIFEST */

#endif /* SIMPLE_MANIFEST_IMPL_H_ */
