/** \file manifest.h
 * \brief Manifest identifying a memory object.
 * \author Antonio Langiu
 * \defgroup Memory
 * \{
 */
#ifndef MANIFEST_H_
#define MANIFEST_H_

#include "common/error.h"
#include "common/types.h"
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
    ITEM(uint8_t*, vendor_signature_r, uint8_t* size) \
    ITEM(uint8_t*, vendor_signature_s, uint8_t* size) \
    ITEM(uint8_t*, server_signature_r, uint8_t* size) \
    ITEM(uint8_t*, server_signature_s, uint8_t* size)

/** The scope of this file is to define the interface of a manifest. It can be
 * implemented using different encodings, but each approach should implement
 * this interface to be usable by the library */

#define DEFINE_GETTER(type, name, ...) \
    type get_##name(const manifest_t* mt, ##__VA_ARGS__);

#define DEFINE_SETTER(type, name, ...) \
    void set_##name(manifest_t* mt, type name, ##__VA_ARGS__);

FOREACH_ITEM(DEFINE_GETTER)
FOREACH_ITEM(DEFINE_SETTER)

#undef DEFINE_GETTER
#undef DEFINE_SETTER

/** 
 * \brief Print manifest known values.
 * \param mt Pointer to a manifest structure.
 */
void print_manifest(const manifest_t* mt);


pull_error verify_manifest_vendor(manifest_t* mt, digest_func f, const uint8_t *pub_x,
                                        const uint8_t *pub_y, ecc_curve curve);
pull_error verify_manifest_server(manifest_t* mt, digest_func f, const uint8_t *pub_x,
                                        const uint8_t *pub_y, ecc_curve curve);

pull_error sign_manifest_vendor(manifest_t* mt, digest_func f, const uint8_t *private_key,
                                    uint8_t* signature_buffer, ecc_curve curve);
pull_error sign_manifest_server(manifest_t* mt, digest_func f, const uint8_t *private_key,
                                    uint8_t* signature_buffer, ecc_curve curve);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} MANIFEST_H_ */
