/** \file receiver.h
 * \brief Set of functions to download an update into a memory object.
 * \author Antonio Langiu
 * \defgroup Network
 * \brief This module includes component to interact with the network.
 *
 * It relies on the transport interface that must be implemented by
 * the developer using this library.
 * \{
 */
#ifndef RECEIVER_H_
#define RECEIVER_H_

#include "memory/memory_objects.h"
#include "network/transport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** 
 * Receiver context used to hold data for the receiver function
 */
typedef struct receiver_ctx_ {
    const char* resource;
    mem_object* obj;
    metadata mt;
    int metadata_received;
    pull_error err;
    uint8_t num_err;
    txp_ctx* txp;
    uint32_t expected;
    uint8_t firmware_received;
    uint32_t start_offset;
    uint32_t received;
} receiver_ctx;

/** 
 * \brief Open the receiver context.
 * This function start the connection with the backend. It uses a
 * transport object to communicate with it and needs a string
 * rappresenting the resource we want to receive from the backend.
 * It stores the received content into a memory object.
 * 
 * \param ctx The receiver context that should be passed to every receiver
 * function.
 * \param txp The transport object. It must be already initialized.
 * \param resource The resource we want to download from the backend.
 * \param id Memory object used to store the received data.
 * \param obj The memory object structure defined by the programmer.
 * 
 * \returns PULL_SUCCESS in case the receiver was correcly initialized or
 * the specific error otherwise.
 */
pull_error receiver_open(receiver_ctx* ctx, txp_ctx* txp, const char* resource, obj_id id, mem_object* obj);

/** 
 * \brief Receive and store a chunk of the update into the memory object.
 *
 * \param ctx The previously initialized receiver context.
 * 
 * \returns PULL_SUCCESS in case the chunk was correcly downloaded and stored
 * or the specific error otherwise.
 */
pull_error receiver_chunk(receiver_ctx* ctx);

/** 
 * \brief Close the receiver context and close the connection with the server.
 * 
 * \param ctx The receiver context to close.
 * 
 * \returns PULL_SUCCESS in case the context was correcly closed or the
 * specific error otherwise.
 */
pull_error receiver_close(receiver_ctx* ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} RECEIVER_H_ */
