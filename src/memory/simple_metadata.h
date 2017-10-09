#ifndef SIMPLE_METADATA_H_
#define SIMPLE_METADATA_H_

#include "memory/metadata.h"

#define SIMPLE_METADATA 1
#ifdef SIMPLE_METADATA

void print_metadata_impl(const metadata* mt);

version_t get_version_impl(const metadata* mt);
platform_t get_platform_impl(const metadata* mt);
address_t get_size_impl(const metadata* mt);
address_t get_offset_impl(const metadata* mt);

void set_version_impl(metadata* mt, version_t version);
void set_platform_impl(metadata* mt, platform_t platform);
void set_size_impl(metadata* mt, address_t size);
void set_offset_impl(metadata* mt, address_t offset);

/* Considering that the manifest of part of it must
 * be included in the signature of the binary, to grant
 * that the manifest is related to that particular object,
 * this function provides a pointer and the size of the
 * manifest part that should be included while calculating
 * the digest
 */
int to_digest_impl(metadata* mt, void* buffer, size_t buffer_len);

#endif /* SIMPLE_METADATA_H_ */

#endif /* SIMPLE_METADATA_H_ */
