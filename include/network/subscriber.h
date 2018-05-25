/** \file subscriber.h
 * \brief Set of functions to check the presence of an update.
 * \author Antonio Langiu
 * \defgroup Network
 * \{
 */
#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include <common/libpull.h>
#include "network/transport.h"
#include "memory/manifest.h"
#include "memory/memory.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    txp_ctx* txp;
    const char* resource;
    uint8_t has_updates;
    version_t current_version;
} subscriber_ctx;

/** 
 * \brief Callback handling the data received by the server. There
 * is a default implementation of this callback but it can be overridden
 * by the user by passing its own callback to the check update function.
 * 
 * \param err Error received by the calling function. PULL_SUCCESS if no
 * error.
 * \param data Data received by the network. NULL if error.
 * \param len Lenght of the received data. 0 if error.
 * \param more Pointer passed during initialization of the transport object.
 * \note The more parameter can be useful to pass receive some structure
 * from the function creating the trasnport when the callback is called.
 */
void subscriber_cb(pull_error err, const char* data, int len, void* more);

/** 
 * \brief Subscribe to a backend for updates.
 * This functions initialize the subsciber context and subscribe to a
 * specific resource. It requires an already initialized transport context.
 *
 * \param ctx A pointer to the subscriber.
 * \param txp An already opended transport object.
 * \param resource A string rapresenting the resource.
 * \param obj_t A temporary memory object.
 * 
 * \returns PULL_SUCCESS on success or the specific error otherwise.
 */
pull_error subscribe(subscriber_ctx* ctx, txp_ctx* txp, const char* resource, mem_object_t* obj_t);

/** 
 * \brief Check the presence of an update.
 * This blocking function perform a request to the specified backend and resource
 * and handles the response using the provided callback. An already defined
 * callback is provided with the library, however you can define your own
 * logic matching the protocol used in your server.
 * \param ctx An already initialized ubscriber context.
 * \param cb The callback.
 * 
 * \returns PULL_SUCCESS on success or the specific error otherwise.
 */
pull_error check_updates(subscriber_ctx* ctx, callback cb);

/** 
 * \brief Unsubscribe from the backend. This function closes context,
 * however does not closes the transport tham must be closed by the
 * caller.
 * 
 * \param ctx The subscriber context to close.
 * 
 * \returns PULL_SUCCESS on success or the specific error otherwise.
 */
pull_error unsubscribe(subscriber_ctx* ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} SUBSCRIBER_H_ */
