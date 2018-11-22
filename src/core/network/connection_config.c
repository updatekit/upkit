#include <libpull/network/connection_config.h>

inline pull_error conn_config(conn_config_t* cfg, char* endpoint, uint16_t port,
                          conn_type type, void* conn_data) {
      cfg->endpoint = endpoint;
      cfg->port = port;
      cfg->connection_type = type;
      cfg->conn_data = conn_data;
      return PULL_SUCCESS;
  }
