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
#include <memory/memory.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern const mem_slot_t memory_slots[];
extern const version_t running_version;

/** OBJ_END defines the final value used to stop the cicle on the
 * memory objects. */
#define OBJ_END -1

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} EXTERNAL_H_ */
