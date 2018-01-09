/* This module is in charge of receiving the firmware form the server.
 * It sends to the server the offset of the binary it wants to receive
 * and the server starts sending from that offset. This is not a common
 * approach on CoAP where a better solution would be based on the block
 * numebers but, this receiver will not be compatible just with coap
 * but with any receiver_txp protocol. The blockwise transfer of CoAP is
 * still used but, if for some reason the connection is broken then
 * the receiver will not send to the server the last block number
 * but instead the binary offset from which it wants to start a new
 * blockwise request */
#include <string.h>

#include "network/receiver.h"
#include "memory/memory_objects.h"
#include "memory/memory.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/async.h"

#define MAX_RECEIVER_ERRORS 10

static void handler(pull_error txp_err, const char* data, int len, void* more) {
    receiver_ctx* ctx = (receiver_ctx*) more;
    if (txp_err) {
        log_error(txp_err, "Network failure\n");
        ctx->err = NETWORK_ERROR; 
        break_loop(ctx->txp);
        return;
    }
    if (len <= 0) {
        log_error(NETWORK_ERROR, "The received lenght (%d) is invalid\n", len);
        ctx->err = NETWORK_ERROR; 
        break_loop(ctx->txp);
        return;
    }
    if (memory_write(ctx->obj, (const uint8_t*) data, len, ctx->received) != len) {
        log_error(MEMORY_WRITE_ERROR, "Failure writing the received data\n");
        ctx->err = MEMORY_WRITE_ERROR;
        break_loop(ctx->txp);
        return;
    }
    if (!ctx->metadata_received) {
        log_debug("Metadata still not received\n");
        int16_t missing = sizeof(metadata)-ctx->received;
        if (missing <= 0) {
            log_debug("Metadata received\n");
            print_metadata(&ctx->mt);
            ctx->expected = get_size(&ctx->mt)+get_offset(&ctx->mt);
            // TODO find a way to check if the firmware to receive is
            // bigger than the object size
            if (get_size(&ctx->mt) <= 0 /* ||ctx->mt.vendor.size > MAX_FIRMWARE_SIZE-sizeof(metadata)*/) {
                log_debug("Received an invalid size %d, aborting\n", (int) ctx->mt.vendor.size);
                ctx->err = INVALID_SIZE_ERROR;
                break_loop(ctx->txp);
                return;
            }
            ctx->metadata_received = 1;
        } else {
            uint8_t* mt_ptr = (uint8_t*) &ctx->mt;
            memcpy(mt_ptr+ctx->received, data, ctx->received+len<=sizeof(metadata)? len: missing);
        }
    }
    ctx->received+=len;
    if (ctx->metadata_received) {
        log_info("Received %lu bytes. Expected %lu bytes\r", ctx->received, ctx->expected);
        if (ctx->received == ctx->expected) {
            ctx->firmware_received = 1;
            ctx->err = PULL_SUCCESS;
            break_loop(ctx->txp);
        } else if (ctx->received > ctx->expected) {
            ctx->err = INVALID_SIZE_ERROR;
            log_error(INVALID_SIZE_ERROR, "Error receiving the firmware\n");
            break_loop(ctx->txp);
            return;
        }
    }
}

pull_error receiver_open(receiver_ctx* ctx, txp_ctx* txp, const char* resource, obj_id i, mem_object* obj) {
    memset(ctx, 0, sizeof(receiver_ctx));
    ctx->txp = txp;
    ctx->err = PULL_SUCCESS;
    ctx->obj = obj;
    ctx->resource = resource;
    log_info("Receiving on memory object %d\n", i);
    pull_error err = memory_open(ctx->obj, i);
    if (err) {
        log_error(err, "Impossible to open the object %d\n", i);
        return RECEIVER_OPEN_ERROR;
    }
    ctx->firmware_received = 0;
    return PULL_SUCCESS;
}

pull_error receiver_chunk(receiver_ctx* ctx) {
    pull_error err = txp_on_data(ctx->txp, handler, ctx);
    if (err) {
        log_error(err, "Failure setting receiver callback\n");
        return RECEIVER_CHUNK_ERROR;
    }
    ctx->start_offset = ctx->received;
    switch(ctx->err) {
        // Recoverable errors
        case MEMORY_WRITE_ERROR:
        case NETWORK_ERROR:
            if (ctx->num_err >= MAX_RECEIVER_ERRORS) {
                return ctx->err;
            }
            ctx->num_err = PULL_SUCCESS;
            break;
        // Not recoverable error. The upgrade process should start again
        case INVALID_SIZE_ERROR:
            return ctx->err;
        default: /* ignore all the other cases */ break;
    }
    err = txp_request(ctx->txp, GET_BLOCKWISE2, ctx->resource, (const char*) &ctx->start_offset, sizeof(uint32_t));
    if (err) {
        log_error(err, "Failure setting receiver request\n");
        return RECEIVER_CHUNK_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error receiver_close(receiver_ctx* ctx) {
    pull_error err = memory_close(ctx->obj);
    if (err) {
        log_error(err, "Failure closing memory\n");
        return RECEIVER_CLOSE_ERROR;
    }
    return PULL_SUCCESS;
}
