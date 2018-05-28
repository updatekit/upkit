/** \file async_interface.h
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
#ifndef LIBPULL_NETWORK_ASYNC_INTERFACE_H_
#define LIBPULL_NETWORK_ASYNC_INTERFACE_H_

#include <libpull/common.h>
#include <libpull/network/connection_interface.h>

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
#endif /* \} LIBPULL_NETWORK_ASYNC_INTERFACE_H_ */
