#include "memory/manifest.h"
#include <string.h>

#define DEFINE_GETTER_IMPL(type, name) \
    type get_##name##_impl(const manifest_t* mt);

#define DEFINE_SETTER_IMPL(type, name) \
    void set_##name##_impl(manifest_t* mt, type name);

#define DEFINE_GETTER_IMPL_MEMORY(type, name) \
    type get_##name##_impl(const manifest_t* mt, uint8_t* size);

#define DEFINE_SETTER_IMPL_MEMORY(type, name) \
    int set_##name##_impl(manifest_t* mt, type name, uint8_t size);

FOREACH_ITEM(DEFINE_GETTER_IMPL)
FOREACH_ITEM(DEFINE_SETTER_IMPL)

#undef DEFINE_GETTER_IMPL
#undef DEFINE_SETTER_IMPL
#undef DEFINE_GETTER_IMPL_MEMORY
#undef DEFINE_SETTER_IMPL_MEMORY

#define IMPLEMENT_GETTER(type, name) \
    type get_##name(const manifest_t* mt) { \
        return get_##name##_impl(mt); \
    }

#define IMPLEMENT_SETTER(type, name) \
    void set_##name(manifest_t* mt, type name) { \
        set_##name##_impl(mt, name); \
    }

#define IMPLEMENT_GETTER_MEMORY(type, name) \
    type get_##name(const manifest_t* mt, uint8_t* size) { \
        return get_##name##_impl(mt, size); \
    }

#define IMPLEMENT_SETTER_MEMORY(type, name) \
    int set_##name(manifest_t* mt, type name, uint8_t size) { \
        return set_##name##_impl(mt, name, size); \
    }

FOREACH_ITEM(IMPLEMENT_GETTER)
FOREACH_ITEM(IMPLEMENT_SETTER)

#undef IMPLEMENT_GETTER
#undef IMPLEMENT_SETTER
#undef IMPLEMENT_GETTER_MEMORY
#undef IMPLEMENT_SETTER_MEMORY


pull_error verify_manifest_vendor_impl(manifest_t* mt, digest_func f, const uint8_t *pub_x,
            const uint8_t *pub_y, ecc_func_t ef);
pull_error verify_manifest_server_impl(manifest_t* mt, digest_func f, const uint8_t *pub_x,
        const uint8_t *pub_y, ecc_func_t ef);
pull_error sign_manifest_vendor_impl(manifest_t* mt, digest_func f, const uint8_t *private_key,
        uint8_t* signature_buffer, ecc_func_t ef);
pull_error sign_manifest_server_impl(manifest_t* mt, digest_func f, const uint8_t *private_key,
        uint8_t* signature_buffer, ecc_func_t ef);

identity_t get_identity_impl(const manifest_t* mt);
void set_identity_impl(manifest_t* mt, identity_t identity);

identity_t get_identity(const manifest_t* mt) {
    return get_identity_impl(mt);
}
void set_identity(manifest_t* mt, identity_t identity) {
    set_identity_impl(mt, identity);
}

void print_manifest_impl(const manifest_t* mt);

void print_manifest(const manifest_t *mt) {
    print_manifest_impl(mt);
}

pull_error verify_manifest_vendor(manifest_t* mt, digest_func f, const uint8_t *pub_x,
        const uint8_t *pub_y, ecc_func_t ef) {
    return verify_manifest_vendor_impl(mt, f, pub_x, pub_y, ef);
}
pull_error verify_manifest_server(manifest_t* mt, digest_func f, const uint8_t *pub_x,
        const uint8_t *pub_y, ecc_func_t ef) {
    return verify_manifest_server_impl(mt, f, pub_x, pub_y, ef);
}
pull_error sign_manifest_vendor(manifest_t* mt, digest_func f, const uint8_t *private_key, 
        uint8_t* signature_buffer, ecc_func_t ef) {
    return sign_manifest_vendor_impl(mt, f, private_key, signature_buffer, ef);
}
pull_error sign_manifest_server(manifest_t* mt, digest_func f, const uint8_t *private_key,
        uint8_t* signature_buffer, ecc_func_t ef) {
    return sign_manifest_server_impl(mt, f, private_key, signature_buffer, ef);
}
