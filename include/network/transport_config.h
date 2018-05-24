#ifndef TRANSPORT_CONFIG_H_
#define TRANSPORT_CONFIG_H_

typedef struct {
     char* endpoint;
     uint16_t port;
     conn_type connection_type;
     void* conn_data;
     char* resource;
 } conn_config_t;

inline pull_error conn_config(conn_config_t* cfg, char* endpoint, uint16_t port,
                         conn_type type, void* conn_data, char* resource);

#endif /* TRANSPORT_CONFIG_H_ */
