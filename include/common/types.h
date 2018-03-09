/** \file types.h
 * \brief Types to be used for consistency in the whole library.
 * \author Antonio Langiu
 * \defgroup Common
 * \{
 */
#ifndef TYPES_H_
#define TYPES_H_

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

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} TYPES_H_ */
