#include "memory/manifest.h"
#include "simple_manifest_impl.h"
#include <string.h>

version_t get_version(const manifest_t *mt) {
  return get_version_impl(mt);
}
platform_t get_platform(const manifest_t *mt) {
  return get_platform_impl(mt);
}
address_t get_size(const manifest_t *mt) { 
    return get_size_impl(mt); 
}
address_t get_offset(const manifest_t *mt) {
  return get_offset_impl(mt);
}
uint8_t *get_server_key_x(const manifest_t *mt) {
  return get_server_key_x_impl(mt);
}
uint8_t *get_server_key_y(const manifest_t *mt) {
  return get_server_key_y_impl(mt);
}
uint8_t *get_digest(const manifest_t *mt) { 
    return get_digest_impl(mt); 
}
uint8_t* get_vendor_signature_r(const manifest_t *mt, size_t* size) {
    return get_vendor_signature_r_impl(mt, size);
}
uint8_t* get_vendor_signature_s(const manifest_t *mt, size_t* size) {
    return get_vendor_signature_s_impl(mt, size);
}
uint8_t* get_server_signature_r(const manifest_t *mt, size_t* size) {
    return get_server_signature_r_impl(mt, size);
}
uint8_t* get_server_signature_s(const manifest_t *mt, size_t* size) {
    return get_server_signature_s_impl(mt, size);
}

void set_version(manifest_t *mt, version_t version) {
    set_version_impl(mt, version);
}
void set_platform(manifest_t *mt, platform_t platform) {
    set_platform_impl(mt, platform);
}
void set_size(manifest_t *mt, address_t size) {
    set_size_impl(mt, size);
}
void set_offset(manifest_t *mt, address_t offset) {
    set_offset_impl(mt, offset);
}
void set_server_key_x(manifest_t *mt, uint8_t *server_key_x) {
    set_server_key_x_impl(mt, server_key_x);
}
void set_server_key_y(manifest_t *mt, uint8_t *server_key_y) {
    set_server_key_y_impl(mt, server_key_y);
}
void set_digest(manifest_t *mt, uint8_t *digest) {
    set_digest_impl(mt, digest);
}
void set_vendor_signature_r(manifest_t *mt, uint8_t* vendor_signature_r, size_t *size) {
    set_vendor_signature_r_impl(mt, vendor_signature_r, size);
}
void set_vendor_signature_s(manifest_t *mt, uint8_t* vendor_signature_r, size_t *size) {
    set_vendor_signature_s_impl(mt, vendor_signature_r, size);
}
void set_server_signature_r(manifest_t *mt, uint8_t* server_signature_r, size_t *size) {
    set_server_signature_r_impl(mt, server_signature_r, size);
}
void set_server_signature_s(manifest_t *mt, uint8_t* server_signature_r, size_t *size) {
    set_server_signature_s_impl(mt, server_signature_r, size);
}

void print_manifest(const manifest_t *mt) {
    print_manifest_impl(mt);
}

size_t get_server_digest_buffer(const manifest_t *mt, void **buffer) {
    return get_server_digest_buffer_impl(mt, buffer);
}

size_t get_vendor_digest_buffer(const manifest_t *mt, void **buffer) {
    return get_vendor_digest_buffer_impl(mt, buffer);
}
