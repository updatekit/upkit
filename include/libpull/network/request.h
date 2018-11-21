#include <libpull/common.h>
#include <libpull/network/connection_interface.h>
#include <libpull/network/fsm.h>

typedef struct request_ctx_t {
    fsm_ctx_t* fsmc;
    conn_ctx* conn;
    pull_error err;
} request_ctx_t;

void request_cb(pull_error err, const char* data, int len, void* more);

