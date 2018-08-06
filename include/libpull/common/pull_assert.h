/** \file pull_assert.h
 * \brief Assert wrapper used by the library.
 * \author Antonio Langiu
 * \defgroup com_passert
 * \{
 */
#ifndef LIBPULL_COMMON_ASSERT_H_
#define LIBPULL_COMMON_ASSERT_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if ENABLE_ASSERT == 1
#include <assert.h>
#define PULL_ASSERT(cond) assert(cond);
#else
#define PULL_ASSERT(cond)

#endif /* ENABLE_ASSERT == 1 */

#ifdef __cplusplus
 }
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_ASSERT_H_ */
