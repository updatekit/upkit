/** \file receiver.h
 * \author Antonio Langiu
 * \defgroup net_rec
 * \brief This module includes component to interact with the network.
 *
 * It relies on the connection interface that must be implemented by
 * the developer using this library.
 * \{
 */
#ifndef LIBPULL_NETWORK_RECEIVER_H_
#define RECEIVER_H_

#include <libpull/common.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    udid_t udid;
    nonce_t nonce;
    version_t version;
} receiver_msg_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_NETWORK_RECEIVER_H_ */
