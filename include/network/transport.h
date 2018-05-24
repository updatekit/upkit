/** \file transport.h
 * \brief Interface to connect with the network.
 * \author Antonio Langiu
 * \defgroup Transport
 * \brief Interface to connect with the network.
 *
 * This interface must be defined by the library user to match
 * the specific application and platform requirements.
 * The implementation of the following functions is mandatory,
 * however if a method is not supported by the specific
 * transport library the NOT_IMPLEMENTED error must be returned.
 * The interface assumes that a REST protocol is used, such as
 * CoAP or HTTP.
 * The implementation of this module will be used by the component
 * of the Network module to check and download the available firmware.
 * \{
 */
#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include "common/libpull.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Verbs supported by the REST transport */
typedef enum rest_method {
    GET, /** Standard REST verb */
    PUT, /** Standard REST verb */
    POST, /** Standard REST verb */
    DELETE, /** Standard REST verb */
    OPTIONS, /** Standard REST verb */
    GET_BLOCKWISE2 /** CoAP specific method */
} rest_method;

/* Supported connection types. You can however define your own
  * connection type and pass it to the txp_init function, with
  * its own data. If you are implementing a connection type
  * defined here you can find some predefined structs in
  * security/secure_transport.h. */
 typedef enum conn_type {
     PULL_TCP,
     PULL_UDP,
     PULL_DTLS_PSK,
     PULL_DTLS_ECDH
 } conn_type;

typedef struct txp_ctx_ txp_ctx;

/** 
 * \brief Init the transport context.
 * This functions initialize the transport context and start the connection with
 * the backend specified in the parameters.
 * \param ctx
 * \param addr Backend address.
 * \param port Backend port.
 * \param type Connection type. This enumberation can be defined in the
 * interface implementation. (i.e., TCP, UDP, DTLS, etc..);
 * \param data This raw pointer stores data for the specific connection type
 * (e.g., keys for a DTLS connection).
 * 
 * \returns PULL_SUCCESS if success or the specific error otherwise.
 */
pull_error txp_init(txp_ctx* ctx, const char* addr, uint16_t port, conn_type type, void* data);

/** 
 * \brief Set the callback to be called in case of a new event.
 * The callback is related to the connection and not the single request. This
 * means that all the requests must be handled by the same callback. This is not
 * a problem in the way the transport module is used by the library because
 * each connection is used just for a specific operation and, in case you want
 * to reuse a connection, you must be sure that all the previous requests has
 * been satisfied.
 * 
 * \param ctx An alredy initialized transport context.
 * \param handler The callback handler.
 * \param more A pointer that will be passed to the callback every time it is
 * called.
 * 
 * \returns PULL_SUCCESS if the callback has been correcly setted.
 */
pull_error txp_on_data(txp_ctx* ctx, callback handler, void* more);

/** 
 * \brief Perform a request to the backend.
 * The request is performed using the method, the resource and the data specified.
 * 
 * \param ctx An already initialized transport context.
 * \param method The rest method to perform the request (e.g., GET, PUT, etc).
 * \param resource The REST resource.
 * \param data (Optional) The data to be sent or NULL.
 * \param length The lenght of the data or 0;
 * 
 * \returns PULL_SUCCESS if the request was correcly sent or the specific error
 * otherwise.
 */
pull_error txp_request(txp_ctx* ctx, rest_method method, const char* resource, const char* data, uint16_t length);

/** 
 * TODO TO BE IMPLEMENTED
 */
pull_error txp_observe(txp_ctx* ctx, const char* resource, const char* token, uint8_t token_length);

/** 
 * \brief Close the transport connection.
 * This function must free all the resources and close the connection with the
 * server. 
 * 
 * \param ctx The transport context to close.
 */
void txp_end(txp_ctx* ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} _TRANSPORT_H */
