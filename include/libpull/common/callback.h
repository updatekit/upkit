/** \file callback.h
  * \brief Declaration of the callback used in the library to handle
  * with network events.
  * \author Antonio Langiu
  * \defgroup Common
  * \{
  */
#ifndef LIBPULL_COMMON_CALLBACK_H_
#define LIBPULL_COMMON_CALLBACK_H_

#include <libpull/common/error.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** 
 * \brief Callback used to handle network events.
 * \param err Error received or PULL_SUCCESS if no error.
 * \param data Received data.
 * \param len Lenght of received data.
 * \param more Raw pointer passed during connection initialization.
 */
typedef void (* callback)(pull_error err, const char* data, int len, void* more);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_CALLBACK_H_ */
