#ifndef _SUBSCRIBER_H
#define _SUBSCRIBER_H

#include "network/transport.h"
#include "memory/metadata.h"
#include "memory/memory.h"

typedef struct {
    txp_ctx* txp;
    const char* resource;
    uint8_t has_updates;
    metadata mt;
} subscriber_ctx;

/** Generic callback for subscribe **/
void subscriber_cb(pull_error err, const char* data, int len, void* more);

pull_error subscribe(subscriber_ctx* ctx, txp_ctx* txp, const char* resource, mem_object* obj_t);

pull_error check_updates(subscriber_ctx* ctx, callback cb);

pull_error unsubscribe(subscriber_ctx* ctx);

#endif
