#include <libpull/pipeline/buffer.h>
#include <libpull/common.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_LEN 1000

#define BUFFER_USED (ctx->bufferp - ctx->buffer)
#define MIN(A,B) ((A<B)? A: B)

int buffer_pipeline(pipeline_ctx_t* c, uint8_t* buf, int l) {
    log_debug("PIP: Buffering %d bytes\n", l);
    struct buffer_ctx* ctx = (struct buffer_ctx*) c->stage_data;
    uint8_t* bufp = buf;

    while (bufp - (buf+l)) {
        int empty = BUFFER_LEN - BUFFER_USED;
        if (empty > 0) {
            int min = MIN((buf+l)-bufp, empty);
            memcpy(ctx->bufferp, bufp, min);
            bufp += min;
            ctx->bufferp += min;
        } else if (empty == 0) {
            c->next_stage(c->next_ctx, ctx->buffer, BUFFER_USED);
            ctx->bufferp = ctx->buffer;
        } else {
            // This is a fatal error since the bufferp must never be higher than
            // buffer+sizeof(buffer). This means we are writing over the buffer
            // boundaries.
            PULL_ASSERT(false);
        } }
    return l;
}

int pipeline_buffer_clear(pipeline_ctx_t* c) {
    struct buffer_ctx* ctx = (struct buffer_ctx*) c->stage_data;
    c->next_stage(c->next_ctx, ctx->buffer, BUFFER_USED);
    return 0;
}

