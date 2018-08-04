/** \file types.h
 * \brief Types to be used for consistency in the whole library.
 * \author Antonio Langiu
 * \defgroup Common
 * \{
 */
#ifndef LIBPULL_COMMON_TYPES_H_
#define LIBPULL_COMMON_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

/** Version type to be used across the library. */
typedef uint16_t version_t;
/** Platform type to be used across the library */
typedef uint16_t platform_t;
/** Address type to be used across the library */
typedef uint32_t address_t;
/** Identifier for the memory objects. It supports at most 255 objects */
/** This must be a signed integer since negative values are used to define
 * an invalid object */
typedef int8_t mem_id_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_TYPES_H_ */
