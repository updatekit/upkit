#include "config.h"
#include "memory/manifest.h"

#ifndef SERVER_H_
#define SERVER_H_

typedef struct server_ctx_t {
    char addr_str[MAX_ADDR_LEN];
    coap_log_t log_level;
    char assets_dir[MAX_PATH_LEN];
    manifest_t mt;
    struct coap_resource_t* resource_version;
    const unsigned char* mapped_file;
    manifest_t* mapped_file_manifest;
    size_t mapped_file_len;
    /// TODO add a server state instead of failing. Just return a 500 error to
    // the client
} server_ctx_t;

#endif
