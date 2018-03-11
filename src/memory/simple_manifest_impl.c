#include "simple_manifest_impl.h"
#include "common/logger.h"
#include <string.h>

#ifdef SIMPLE_MANIFEST

version_t get_version_impl(const manifest_t* mt) {
    return mt->vendor.version;
}
platform_t get_platform_impl(const manifest_t* mt) {
    return mt->vendor.platform;
}
address_t get_size_impl(const manifest_t* mt) {
    return mt->vendor.size;
}
address_t get_offset_impl(const manifest_t* mt) {
    return mt->vendor.offset;
}
uint8_t* get_digest_impl(const manifest_t* mt) {
    return (uint8_t*) mt->vendor.digest;
}
uint8_t* get_server_key_x_impl(const manifest_t* mt) {
    return (uint8_t*) mt->vendor.server_key_x;
}
uint8_t* get_server_key_y_impl(const manifest_t* mt) {
    return (uint8_t*) mt->vendor.server_key_y;
}

void set_version_impl(manifest_t* mt, version_t version) {
    mt->vendor.version = version;
}
void set_platform_impl(manifest_t* mt, platform_t platform) {
    mt->vendor.platform = platform;
}
void set_size_impl(manifest_t* mt, address_t size) {
    mt->vendor.size = size;
}
void set_offset_impl(manifest_t* mt, address_t offset) {
    mt->vendor.offset = offset;
}
void set_digest_impl(manifest_t* mt, uint8_t* digest) {
    // TODO    
}
void set_server_key_x_impl(manifest_t* mt, uint8_t* server_key_x) {
    // TODO    
}
void set_server_key_y_impl(manifest_t* mt, uint8_t* server_key_y) {
    // TODO    
}

uint8_t* get_vendor_signature_r_impl(const manifest_t *mt, size_t* size) {
    return (uint8_t*) mt->vendor_signature_r;
}
uint8_t* get_vendor_signature_s_impl(const manifest_t *mt, size_t* size) {
    return (uint8_t*) mt->vendor_signature_s;
}
uint8_t* get_server_signature_r_impl(const manifest_t *mt, size_t* size) {
    return (uint8_t*) mt->server_signature_r;
}
uint8_t* get_server_signature_s_impl(const manifest_t *mt, size_t* size) {
    return (uint8_t*) mt->server_signature_s;
}


void set_vendor_signature_r_impl(manifest_t *mt, uint8_t* vendor_signature_r, size_t *size) {
    memcpy(mt->vendor_signature_r, vendor_signature_r, *size);
}
void set_vendor_signature_s_impl(manifest_t *mt, uint8_t* vendor_signature_s, size_t *size) {
    memcpy(mt->vendor_signature_s, vendor_signature_s, *size);
}
void set_server_signature_r_impl(manifest_t *mt, uint8_t* server_signature_r, size_t *size) {
    memcpy(mt->server_signature_r, server_signature_r, *size);
}
void set_server_signature_s_impl(manifest_t *mt, uint8_t* server_signature_s, size_t *size) {
    memcpy(mt->server_signature_s, server_signature_s, *size);
}

size_t get_vendor_digest_buffer_impl(const manifest_t* mt, void** buffer) {
    *buffer = (void*) &(mt->vendor);
    return sizeof(vendor_manifest_t);
}

size_t get_server_digest_buffer_impl(const manifest_t* mt, void** buffer) {
    *buffer = (void*) &(mt->server);
    return sizeof(server_manifest_t);
}

void print_manifest_impl(const manifest_t* mt) {
    log_info("Platform: %04x\n", mt->vendor.platform);
    log_info("Version: %04x\n", mt->vendor.version);
    log_info("Size: %d\n", (int) mt->vendor.size);
}

#endif /* SIMPLE_MANIFEST */
