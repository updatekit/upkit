#ifndef PULL_SERVER_H_
#define PULL_SERVER_H_

#include "config.h"
#include "memory/manifest.h"

typedef struct server_ctx_t {
    char addr_str[MAX_ADDR_LEN];
    coap_log_t log_level;
    char firmware_name[MAX_PATH_LEN];
    struct coap_resource_t* resource_version;
    const unsigned char* mapped_file;
    manifest_t* mapped_file_manifest;
    size_t mapped_file_len;
} server_ctx_t;

#endif /* PULL_SERVER_H_ */
