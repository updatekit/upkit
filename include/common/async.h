#include "network/transport.h"
#ifndef _ASYNC_H
#define _ASYNC_H

void loop_once(txp_ctx* ctx, uint32_t timeout);
void loop(txp_ctx* ctx, uint32_t timeout);
void break_loop(txp_ctx* ctx);

#endif
