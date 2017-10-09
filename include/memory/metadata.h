#ifndef METADATA_H_
#define METADATA_H_

#include <stdint.h>
#include <stddef.h>
#include "common/error.h"

typedef uint16_t version_t;
typedef uint16_t platform_t;
typedef uint32_t address_t;

#define SIMPLE_METADATA 1
#ifdef SIMPLE_METADATA

typedef struct {
    address_t size;
    address_t offset;
    version_t version;
    platform_t platform;
    uint8_t server_key_x[32];
    uint8_t server_key_y[32];
} vendor_metadata_t;

typedef struct {
    uint16_t self_checking_flags;
} server_metadata_t;

typedef struct {
    uint8_t server_signature_r[32];
    uint8_t server_signature_s[32];
    server_metadata_t server;
    uint8_t vendor_signature_r[32];
    uint8_t vendor_signature_s[32];
    vendor_metadata_t vendor;
} metadata;

#endif /* SIMPLE_METADATA */

void print_metadata(const metadata* mt);

version_t get_version(const metadata* mt);
platform_t get_platform(const metadata* mt);
address_t get_size(const metadata* mt);
address_t get_offset(const metadata* mt);

void set_version(metadata* mt, version_t version);
void set_platform(metadata* mt, platform_t platform);
void set_size(metadata* mt, address_t size);
void set_offset(metadata* mt, address_t offset);

/* Considering that the manifest of part of it must
 * be included in the signature of the binary, to grant
 * that the manifest is related to that particular object,
 * this function provides a pointer and the size of the
 * manifest part that should be included while calculating
 * the digest
 */
int to_digest(metadata* mt, void* buffer, size_t buffer_len);

#endif // METADATA_H_
