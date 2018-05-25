/** \file callback.h
  * \brief Declaration of the callback used in the library to handle
  * with network events.
  * \author Antonio Langiu
  * \defgroup Common
  * \{
  */
#ifndef CALLBACK_H_
#define CALLBACK_H_

#include "common/libpull.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** 
 * \brief Callback used to handle network events.
 * \param err Error received or PULL_SUCCESS if no error.
 * \param data Received data.
 * \param len Lenght of received data.
 * \param more Raw pointer passed during transport initialization.
 */
typedef void (* callback)(pull_error err, const char* data, int len, void* more);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} CALLBACK_H_ */
