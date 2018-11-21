#include <libpull/network/connection_config.h>

pull_error conn_config(conn_config_t* cfg, char* endpoint, uint16_t port,
                          conn_type type) {
      cfg->endpoint = endpoint;
      cfg->port = port;
      cfg->connection_type = type;
      return PULL_SUCCESS;
  }
