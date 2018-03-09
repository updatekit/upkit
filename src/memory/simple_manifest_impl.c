#include "simple_manifest_impl.h"
#include "common/logger.h"
#include <string.h>

#ifdef SIMPLE_MANIFEST

inline version_t get_version_impl(const manifest_t* mt) {
    return mt->vendor.version;
}
inline platform_t get_platform_impl(const manifest_t* mt) {
    return mt->vendor.platform;
}
inline address_t get_size_impl(const manifest_t* mt) {
    return mt->vendor.size;
}
inline address_t get_offset_impl(const manifest_t* mt) {
    return mt->vendor.offset;
}
inline uint8_t* get_digest_impl(const manifest_t* mt) {
    return (uint8_t*) mt->vendor.digest;
}
inline uint8_t* get_server_key_x_impl(const manifest_t* mt) {
    return (uint8_t*) mt->vendor.server_key_x;
}
inline uint8_t* get_server_key_y_impl(const manifest_t* mt) {
    return (uint8_t*) mt->vendor.server_key_y;
}

inline void set_version_impl(manifest_t* mt, version_t version) {
    mt->vendor.version = version;
}
inline void set_platform_impl(manifest_t* mt, platform_t platform) {
    mt->vendor.platform = platform;
}
inline void set_size_impl(manifest_t* mt, address_t size) {
    mt->vendor.size = size;
}
inline void set_offset_impl(manifest_t* mt, address_t offset) {
    mt->vendor.offset = offset;
}
inline void set_digest_impl(manifest_t* mt, uint8_t* digest) {
    // TODO    
}
inline void set_server_key_x_impl(manifest_t* mt, uint8_t* server_key_x) {
    // TODO    
}
inline void set_server_key_y_impl(manifest_t* mt, uint8_t* server_key_y) {
    // TODO    
}

int to_digest_impl(manifest_t* mt, void* buffer, size_t buffer_len) {
    if (!buffer || buffer_len < sizeof(vendor_manifest_t)) {
        return -1;
    }
    if (!memcpy(buffer, &mt->vendor, sizeof(vendor_manifest_t))) {
        return  -1;
    }
    return sizeof(vendor_manifest_t);
}

void print_manifest_impl(const manifest_t* mt) {
    log_info("Platform: %04x\n", mt->vendor.platform);
    log_info("Version: %04x\n", mt->vendor.version);
    log_info("Size: %d\n", (int) mt->vendor.size);
}

#endif /* SIMPLE_MANIFEST */
