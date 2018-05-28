#include <libpull/network/transport_config.h>

pull_error conn_config(conn_config_t* cfg, char* endpoint, uint16_t port,
                          conn_type type, void* conn_data, char* resource) {
      // TODO validate params
      cfg->endpoint = endpoint;
      cfg->port = port;
      cfg->connection_type = type;
      cfg->conn_data = conn_data;
      cfg->resource = resource;
      return PULL_SUCCESS;
  }
