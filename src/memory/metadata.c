#include <string.h>

#include "memory/metadata.h"
#include "simple_metadata.h"

inline void print_metadata(const metadata* mt) {
    return print_metadata_impl(mt);
}

inline version_t get_version(const metadata* mt) {
    return get_version_impl(mt);
}
inline platform_t get_platform(const metadata* mt) {
    return get_platform_impl(mt);
}
inline address_t get_size(const metadata* mt) {
    return get_size_impl(mt);
}
inline address_t get_offset(const metadata* mt) {
    return get_offset_impl(mt);
}

inline void set_version(metadata* mt, version_t version) {
    set_version_impl(mt, version);
}

inline void set_platform(metadata* mt, platform_t platform) {
    set_platform_impl(mt, platform);
}

inline void set_size(metadata* mt, address_t size) {
    set_size_impl(mt, size);
}

inline void set_offset(metadata* mt, address_t offset) {
    set_offset_impl(mt, offset);
}

int to_digest(metadata* mt, void* buffer, size_t buffer_len) {
    return to_digest_impl(mt, buffer, buffer_len);
}
