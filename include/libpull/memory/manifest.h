/** \file manifest.h
 * \brief Manifest identifying a memory object.
 * \author Antonio Langiu
 * \defgroup mem_man
 * \{
 */
#ifndef LIBPULL_MEMORY_MANIFEST_H_
#define LIBPULL_MEMORY_MANIFEST_H_

#include <libpull/memory/simple_manifest.h>
#include <libpull/common.h>
#include <libpull/security/digest.h>
#include <libpull/security/ecc.h>
#include <libpull/security/safestore.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define FOREACH_ITEM(ITEM) \
    ITEM(version_t, version) \
    ITEM(appid_t, appid) \
    ITEM(address_t, size) \
    ITEM(address_t, offset) \
    ITEM(udid_t, udid) \
    ITEM(nonce_t, nonce) \
    ITEM(version_t, diff_version) \
    ITEM(uint8_t*, server_key_x) \
    ITEM(uint8_t*, server_key_y) \
    ITEM(uint8_t*, digest) \
    /* These other item take another parameter of type uint8_t* size */ \
    ITEM##_MEMORY(uint8_t*, vendor_signature_r) \
    ITEM##_MEMORY(uint8_t*, vendor_signature_s) \
    ITEM##_MEMORY(uint8_t*, server_signature_r) \
    ITEM##_MEMORY(uint8_t*, server_signature_s)

/** The scope of this file is to define the interface of a manifest. It can be
 * implemented using different encodings, but each approach should implement
 * this interface to be usable by the library */

#define DEFINE_GETTER(type, name) \
    type get_##name(const manifest_t* mt);

#define DEFINE_SETTER(type, name) \
    void set_##name(manifest_t* mt, type name);

/* Memory getters return a pointer to the memory buffer and its size */
#define DEFINE_GETTER_MEMORY(type, name) \
    type get_##name(const manifest_t* mt, uint8_t* size);

/* Memory setters return 0 if success or 1 if error */
#define DEFINE_SETTER_MEMORY(type, name) \
    int set_##name(manifest_t* mt, type name, uint8_t size);

FOREACH_ITEM(DEFINE_GETTER)
FOREACH_ITEM(DEFINE_SETTER)

#undef DEFINE_GETTER
#undef DEFINE_SETTER
#undef DEFINE_GETTER_MEMORY
#undef DEFINE_SETTER_MEMORY

/** 
 * \brief Print manifest known values.
 * \param mt Pointer to a manifest structure.
 */
void print_manifest(const manifest_t* mt);

pull_error verify_signature(manifest_t* mt, keystore_t keystore);

#ifdef ENABLE_SIGN
pull_error sign_manifest_vendor(manifest_t* mt, const uint8_t *private_key,
                                    uint8_t* signature_buffer);
pull_error sign_manifest_server(manifest_t* mt, const uint8_t *private_key,
                                    uint8_t* signature_buffer);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_MEMORY_MANIFEST_H_ */
