/** \file external.h
 * \author Antonio Langiu
 * \defgroup com_ext
 * \brief This file includes some external functions that
 * must be defined in the application. This is mandatory as
 * otherwise the final application will fail during the linking
 * process.
 * \{
 */
#ifndef LIBPULL_COMMON_EXTERNAL_H_
#define LIBPULL_COMMON_EXTERNAL_H_

#include <libpull/memory/memory_interface.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** 
 * \brief  This structure contains a list of the memory slots needed
 * by your application. We already have a standard definition but you
 * can define it by yourself and personalize according to your needs.
 * 
 * An example of a correctly implemented memory slot is structure is:
 *
 * ~~~~~{.c}
 *   const mem_slot_t memory_slots[] = {
 *    {
 *        .id = OBJ_1,
 *        .bootable = true,
 *        .loaded = true
 *    },
 *    {
 *        .id = OBJ_2,
 *       .bootable = false,
 *       .loaded = false
 *   },
 *   {OBJ_END}
 *  };
 * ~~~~~
 */
ex *tern const mem_slot_t memory_slots[];

/** \brief OBJ_END must be used to end a memory slot list. Internally 
 * the library uses it to know when to stop cycling over the structure.
 */
#define OBJ_END -1

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_EXTERNAL_H_ */
