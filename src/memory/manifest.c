#include <string.h>
#include "memory/manifest.h"
#include "simple_manifest_impl.h"

#define IMPLEMENT_GETTER(type, name) \
    inline type get_##name(const manifest_t* mt) { \
        return get_##name##_impl(mt); \
    } \

#define IMPLEMENT_SETTER(type, name) \
    inline void set_##name(manifest_t* mt, type name) { \
        set_##name##_impl(mt, name); \
    } \

FOREACH_ITEM(IMPLEMENT_GETTER)

FOREACH_ITEM(IMPLEMENT_SETTER)

inline void print_manifest(const manifest_t* mt) {
    print_manifest_impl(mt);
}

