/** \file callback.h
  * \author Antonio Langiu
  * \defgroup com_cb
  * \{
  */
#ifndef LIBPULL_COMMON_CALLBACK_H_
#define LIBPULL_COMMON_CALLBACK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libpull/common/error.h>

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
