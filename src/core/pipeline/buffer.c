#include <libpull/pipeline/buffer.h>
#include <libpull/common.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_LEN 1000

struct buffer_ctx {
    uint8_t buffer[BUFFER_LEN];
    uint8_t* bufferp;
};

int pipeline_buffer_init(pipeline_ctx_t* c, void* more) {
    static struct buffer_ctx ctx;
    ctx.bufferp = ctx.buffer;
    c->stage_data = &ctx;
    return 0;
}

#define BUFFER_USED (ctx->bufferp - ctx->buffer)
#define MIN(A,B) ((A<B)? A: B)

int pipeline_buffer_process(pipeline_ctx_t* c, uint8_t* buf, int l) {
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
            c->next_func->process(c->next_ctx, ctx->buffer, BUFFER_USED);
            ctx->bufferp = ctx->buffer;
        } else {
            // This is a fatal error since the bufferp must never be higher than
            // buffer+sizeof(buffer). This means we are writing over the buffer
            // boundaries.
            PULL_ASSERT(0);
        } }
    return l;
}

int pipeline_buffer_clear(pipeline_ctx_t* c) {
    struct buffer_ctx* ctx = (struct buffer_ctx*) c->stage_data;
    c->next_func->process(c->next_ctx, ctx->buffer, BUFFER_USED);
    if (c->next_func && c->next_func->clear) {
        c->next_func->clear(c->next_ctx);
    }
    // Do nothing
    return 0;
}

