/** \file manifest.h
 * \brief Manifest identifying a memory object.
 * \author Antonio Langiu
 * \defgroup Memory
 * \{
 */
#define FOREACH_ITEM(ITEM) \
    ITEM(version_t, version) \
    ITEM(platform_t, platform) \
    ITEM(address_t, size) \
    ITEM(address_t, offset) \
    ITEM(uint8_t*, server_key_x) \
    ITEM(uint8_t*, server_key_y) \
    ITEM(uint8_t*, digest)


#ifndef MANIFEST_H_
#define MANIFEST_H_

#include "common/error.h"
#include "common/types.h"
#include "memory/simple_manifest.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The scope of this file is to define the interface of a manifest. It can be
 * implemented using different encodings, but each approach should implement
 * this interface to be usable by the library */

#define DEFINE_GETTER(type, name) \
    type get_##name(const manifest_t* mt);

#define DEFINE_SETTER(type, name) \
    void set_##name(manifest_t* mt, type name);

FOREACH_ITEM(DEFINE_GETTER)
FOREACH_ITEM(DEFINE_SETTER)

#undef DEFINE_GETTER
#undef DEFINE_SETTER

/** 
 * \brief Print manifest known values.
 * \param mt Pointer to a manifest structure.
 */
void print_manifest(const manifest_t* mt);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} MANIFEST_H_ */
