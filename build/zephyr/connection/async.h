#ifndef ASYNC_H_
#define ASYNC_H_

#include <libpull/common.h>
#include <libpull/network.h>

void loop_once(txp_ctx* ctx, uint32_t timeout);

void loop(txp_ctx* ctx, uint32_t timeout);

void break_loop(txp_ctx* ctx);

#endif /* ASYNC_H_ */
