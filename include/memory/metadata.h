/** \file metadata.h
 * \brief Metadata identifying a memory object.
 * \author Antonio Langiu
 * \defgroup Memory
 * \{
 */
#ifndef METADATA_H_
#define METADATA_H_

#include "common/error.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Version type to be used across the library. */
typedef uint16_t version_t;
/** Platform type to be used across the library */
typedef uint16_t platform_t;
/** Address type to be used across the library */
typedef uint32_t address_t; // TODO check for consistency

#define SIMPLE_METADATA 1
#ifdef SIMPLE_METADATA

/** Metadata that must be filled and signed by the vendor */
typedef struct {
    address_t size; /** Size of the object. */
    address_t offset; /** Offset of the blob starting from 0. */
    version_t version; /** Version of the object. */
    platform_t platform; /** Platform number */
    uint8_t server_key_x[32]; /** Public key X component of the server */
    uint8_t server_key_y[32]; /** Public key Y component of the server */
} vendor_metadata_t;

/** Metadata filled by the server */
typedef struct {
    uint16_t self_checking_flags; /** Flags used to perform self checking after update */
} server_metadata_t;

/** General metadata structure */
typedef struct {
    uint8_t server_signature_r[32]; /** R component of server's signature */
    uint8_t server_signature_s[32]; /** S component of server's signature */
    server_metadata_t server; /** Server metadata */
    uint8_t vendor_signature_r[32]; /** R component of vendor's signature */
    uint8_t vendor_signature_s[32]; /** S component of vendor's signature */
    vendor_metadata_t vendor; /** Vendor metadata */
} metadata;

#endif /* SIMPLE_METADATA */

/** 
 * \brief Print metadata known values.
 * \param mt Pointer to a metadata structure.
 */
void print_metadata(const metadata* mt);

/** Getter for the metadata structure. */
version_t get_version(const metadata* mt);

/** Getter for the metadata structure. */
platform_t get_platform(const metadata* mt);

/** Getter for the metadata structure. */
address_t get_size(const metadata* mt);

/** Getter for the metadata structure. */
address_t get_offset(const metadata* mt);

/** Setter for a metadata structure. */
void set_version(metadata* mt, version_t version);

/** Setter for a metadata structure. */
void set_platform(metadata* mt, platform_t platform);

/** Setter for a metadata structure. */
void set_size(metadata* mt, address_t size);

/** Setter for a metadata structure. */
void set_offset(metadata* mt, address_t offset);

/** 
 * \brief Gives the buffer that should
 * be passed to the digest function given a metadata structure.
 * 
 * The manifest (or part of it) must
 * be included in the signature of the binary to make sure
 * that the manifest is related to that particular object.
 * This function provides a pointer to the buffer that should
 * be passed to the digest function and its lenght.
 * The function will check if the lenght of the received buffer
 * is enought big to copy the metadata part.
 * \param mt The metadata object.
 * \param buffer The buffer on which the data will be copied.
 * \param buffer_len The lenght of the buffer.
 * 
 * \returns The lenght of the data copied into the buffer or a negative
 * value in case of error. Consider that the returned value will always
 * be less or equal to the buffer_len parameter.
 */
int to_digest(metadata* mt, void* buffer, size_t buffer_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} METADATA_H_ */
