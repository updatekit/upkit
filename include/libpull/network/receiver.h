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
#ifndef LIBPULL_NETWORK_RECEIVER_H_
#define RECEIVER_H_

#include <libpull/common.h>
#include <libpull/memory/memory_objects.h>
#include <libpull/network/connection_interface.h>
#include <libpull/network/writer.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MESSAGE_VERSION 0x1
typedef struct {
    uint32_t offset;
    uint16_t udid;
    uint16_t random;
    uint8_t msg_version;
} receiver_msg_t;

/** 
 * Receiver context used to hold data for the receiver function
 */
typedef struct receiver_ctx {
    writer_ctx_t wctx;
    receiver_msg_t msg;
    txp_ctx* txp;
    identity_t* identity;
    const char* resource;
    pull_error err;
    uint8_t num_err;
    uint8_t firmware_received;
    uint32_t start_offset;
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
 * \param identity The device identity used for this particular request
 * \param resource The resource we want to download from the backend.
 * \param obj Memory object used to store the received data. It must be opened.
 * 
 * \returns PULL_SUCCESS in case the receiver was correcly initialized or
 * the specific error otherwise.
 */
pull_error receiver_open(receiver_ctx* ctx, txp_ctx* txp, identity_t* identity,
                        const char* resource, mem_object_t* obj);

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
#endif /* \} LIBPULL_NETWORK_RECEIVER_H_ */
