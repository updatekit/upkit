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

size_t get_vendor_digest_buffer_impl(const manifest_t* mt, void** buffer);

size_t get_server_digest_buffer_impl(const manifest_t* mt, void** buffer);

#endif /* SIMPLE_MANIFEST */

#endif /* SIMPLE_MANIFEST_IMPL_H_ */
