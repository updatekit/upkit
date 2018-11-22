/** \file connection_config.h
 * \brief Interface containing functions and structure to configure the
 * connection.
 * \author Antonio Langiu
 * \defgroup net_connconf
 * \{
 */
#ifndef LIBPULL_NETWORK_CONNECTION_CONFIG_H_
#define LIBPULL_NETWORK_CONNECTION_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <libpull/common.h>
#include <libpull/network/connection_interface.h>

typedef struct {
    char* endpoint;
    uint16_t port;
    conn_type connection_type;
    void* conn_data;
} conn_config_t;

pull_error conn_config(conn_config_t* cfg, char* endpoint, uint16_t port, conn_type type, void* conn_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_NETWORK_CONNECTION_CONFIG_H_ */
