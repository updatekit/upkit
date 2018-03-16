/** \file manifest.h
 * \brief Manifest identifying a memory object.
 * \author Antonio Langiu
 * \defgroup Memory
 * \{
 */
#ifndef MANIFEST_H_
#define MANIFEST_H_

#include "common/libpull.h"
#include "memory/simple_manifest.h"
#include "security/digest.h"
#include "security/ecc.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define FOREACH_ITEM(ITEM) \
    ITEM(version_t, version) \
    ITEM(platform_t, platform) \
    ITEM(address_t, size) \
    ITEM(address_t, offset) \
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

identity_t get_identity(const manifest_t* mt);
void set_identity(manifest_t* mt, identity_t identity);

pull_error verify_manifest_vendor(manifest_t* mt, digest_func df, const uint8_t *pub_x,
                                        const uint8_t *pub_y, ecc_func_t ef);
pull_error verify_manifest_server(manifest_t* mt, digest_func df, const uint8_t *pub_x,
                                        const uint8_t *pub_y, ecc_func_t ef);

pull_error sign_manifest_vendor(manifest_t* mt, digest_func df, const uint8_t *private_key,
                                    uint8_t* signature_buffer, ecc_func_t ef);
pull_error sign_manifest_server(manifest_t* mt, digest_func df, const uint8_t *private_key,
                                    uint8_t* signature_buffer, ecc_func_t ef);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} MANIFEST_H_ */
