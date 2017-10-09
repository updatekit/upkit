#ifndef _ASYNC_LIBCOAP_H
#define _ASYNC_LIBCOAP_H

#include "common/settings.h"
#include "common/async.h"
#include "network/transport.h"
#include "transport_libcoap.h"

#include <coap/coap.h>

void loop_once(txp_ctx* ctx, uint32_t timeout);

void loop(txp_ctx* ctx, uint32_t timeout);

void break_loop(txp_ctx* ctx);

#endif // _ASYNC_LIBCOAP_H
