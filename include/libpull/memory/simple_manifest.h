/** \file simple_manifest.h
 * \brief Structure of a simple manifest
 * \author Antonio Langiu
 * \defgroup mem_sman
 * \{
 */
#ifndef LIBPULL_MEMORY_SIMPLE_MANIFEST_H_
#define LIBPULL_MEMORY_SIMPLE_MANIFEST_H_

#include <libpull/common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef SIMPLE_MANIFEST

/** Manifest that must be filled and signed by the vendor */
typedef struct vendor_manifest_t {
    address_t size; /** Size of the object. */
    address_t offset; /** Offset of the blob starting from 0. */
    version_t version; /** Version of the object. */
    appid_t appid; /** App Identifer: it must represent the platform and the application */
    uint8_t digest[32]; /** Digest of the firmware */
    uint8_t server_key_x[32]; /** Public key X component of the server */
    uint8_t server_key_y[32]; /** Public key Y component of the server */
} vendor_manifest_t;

/** Manifest filled by the server */
typedef struct server_manifest_t {
    udid_t udid;
    nonce_t nonce;
    version_t diff_version;
} server_manifest_t;

/** General manifest structure */
typedef struct manifest_t {
    uint8_t vendor_signature_r[32]; /** R component of vendor's signature */
    uint8_t vendor_signature_s[32]; /** S component of vendor's signature */
    vendor_manifest_t vendor; /** Vendor manifest */
    uint8_t server_signature_r[32]; /** R component of server's signature */
    uint8_t server_signature_s[32]; /** S component of server's signature */
    server_manifest_t server; /** Server manifest */
} manifest_t;

#endif /* SIMPLE_MANIFEST */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_MEMORY_SIMPLE_MANIFEST_H_ */
