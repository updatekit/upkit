/** \file async.h
 * \brief Interface to interact with the event loop.
 *
 * The library requires the developer to implement the transport
 * interface to handle the network. Many implementation will be based
 * on an event loop and this interface can be implemented to deal with them.
 * The implemention is optional.
 * \author Antonio Langiu
 * \defgroup Common
 * \{
 */
#ifndef ASYNC_H_
#define ASYNC_H_

#include "network/transport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** 
 * \brief Loop one time and return.
 * \param ctx An already initialized transport context.
 * \param timeout Time to wait before returning if no event is scheduled.
 */
void loop_once(txp_ctx* ctx, uint32_t timeout);

/** 
 * \brief Blocking function to start the loop.
 * \param ctx An already initialized transport context.
 * \param timeout Time to wait before returning if no event is scheduled.
 */
void loop(txp_ctx* ctx, uint32_t timeout);

/** 
 * \brief This function breaks the loop.
 * The execution will continue from the loop() function.
 * \param ctx An already initialized transport context.
 */
void break_loop(txp_ctx* ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} ASYNC_H_ */
