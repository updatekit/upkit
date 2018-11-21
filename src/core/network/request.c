#include <libpull/network.h>
#include <libpull/common.h>

void fsm_cb(pull_error err, const char* data, int len, void* more) {
     request_ctx_t* ctx = (request_ctx_t*) more;
     if (err || data == NULL || len != sizeof(uint16_t)) {
         log_error(err, "Error in the callback\n");
         break_loop(ctx->conn);
         ctx->err = err;
         return;
     }
     ctx->err = fsm(ctx->fsmc, (uint8_t*) data, len);
     if (ctx->err) {
         log_error(ctx->err, "Error in the callback\n");
         break_loop(ctx->conn);
         return;
     }
 }
