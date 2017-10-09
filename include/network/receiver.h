#include "memory/memory_objects.h"
#include "network/transport.h"

#ifndef _RECEIVER_H
#define _RECEIVER_H

typedef struct receiver_ctx_ {
    const char* resource;
    mem_object* obj;
    metadata mt;
    int metadata_received;
    pull_error err;
    uint8_t num_err;
    txp_ctx* txp;
    uint32_t expected;
    uint8_t firmware_received;
    uint32_t start_offset;
    uint32_t received;
} receiver_ctx;

pull_error receiver_open(receiver_ctx* ctx, txp_ctx* txp, const char* resource, obj_id id, mem_object* obj);

pull_error receiver_chunk(receiver_ctx* ctx);

pull_error receiver_close(receiver_ctx* ctx);

#endif
