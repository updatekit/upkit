#ifndef ASYNC_LIBCOAP_H_
#define ASYNC_LIBCOAP_H_

#ifdef WITH_CONNECTION_LIBCOAP

#include <libpull/common.h>
#include <libpull/network.h>

#include "connection_libcoap.h"

#include <coap/coap.h>

void loop_once(conn_ctx* ctx, uint32_t timeout);

void loop(conn_ctx* ctx, uint32_t timeout);

void break_loop(conn_ctx* ctx);

#endif /* WITH_CONNECTION_LIBCOAP */
#endif /* ASYNC_LIBCOAP_H_ */
