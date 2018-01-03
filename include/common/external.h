/** \file external.h
 * \brief This file includes some external functions that
 * must be defined in the application. This is mandatory as
 * otherwise the final application will fail during the linking
 * process.
 * \author Antonio Langiu
 * \defgroup Common
 * \{
 */
#ifndef EXTERNAL_H_
#define EXTERNAL_H_

#include <stdint.h>

 #ifdef __cplusplus
 extern "C" {
 #endif /* __cplusplus */

/** This array of integer identify the memory objects
 * with a valid manifest present in memory.
 * All the value of this array must be positive and
 * must match a valid memory object in the device memory.
 * The last element must be a negative value, or the
 * OBJ_END constant.
 *
 * Example: if you need to have two memory objects to store
 * the updates you can define the variable in this way:
 *
 *     const int8_t memory_objects[] = {OBJ_1, OBJ2, OBJ_END};
 *
 */
extern const int8_t memory_objects[];

/** OBJ_END defines the final value used to stop the cicle on the
 * memory objects. */
#define OBJ_END -1

#ifdef __cplusplus
 }
#endif /* __cplusplus */
#endif /* \} EXTERNAL_H_ */
