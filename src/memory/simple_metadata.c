#include <string.h>

#include "simple_metadata.h"
#include "common/logger.h"

#ifdef SIMPLE_METADATA

void print_metadata_impl(const metadata* mt) {
    log_info("Platform: %04x\n", mt->vendor.platform);
    log_info("Version: %04x\n", mt->vendor.version);
    log_info("Size: %d\n", (int) mt->vendor.size);
}

inline version_t get_version_impl(const metadata* mt) {
    return mt->vendor.version;
}
inline platform_t get_platform_impl(const metadata* mt) {
    return mt->vendor.platform;
}
inline address_t get_size_impl(const metadata* mt) {
    return mt->vendor.size;
}
inline address_t get_offset_impl(const metadata* mt) {
    return mt->vendor.offset;
}

inline void set_version_impl(metadata* mt, version_t version) {
    mt->vendor.version = version;
}

inline void set_platform_impl(metadata* mt, platform_t platform) {
    mt->vendor.platform = platform;
}

inline void set_size_impl(metadata* mt, address_t size) {
    mt->vendor.size = size;
}

inline void set_offset_impl(metadata* mt, address_t offset) {
    mt->vendor.offset = offset;
}

int to_digest_impl(metadata* mt, void* buffer, size_t buffer_len) {
    if (!buffer || buffer_len < sizeof(vendor_metadata_t)) {
        return -1;
    }
    if (!memcpy(buffer, &mt->vendor, sizeof(vendor_metadata_t))) {
        return  -1;
    }
    return sizeof(vendor_metadata_t);
}

#endif /* SIMPLE_METADATA */
