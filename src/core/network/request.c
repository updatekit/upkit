#include <libpull/network.h>
#include <libpull/common.h>

void request_cb(pull_error err, const char* data, int len, void* more) {
     request_ctx_t* ctx = (request_ctx_t*) more;
     if (err) {
        log_err(err, "Network error\n");
        ctx->err = err;
        return;
     }
     if (data == NULL || len < 0) {
         log_error(err, "Invalid data received\n");
         break_loop(ctx->conn);
         ctx->err = err;
         return;
     }
     ctx->err = fsm(ctx->fsmc, (uint8_t*) data, len);
     if (ctx->err) {
         log_error(ctx->err, "Error returned by the FSM\n");
         break_loop(ctx->conn);
         return;
     }
 }
