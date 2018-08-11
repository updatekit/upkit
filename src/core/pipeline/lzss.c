#include <libpull/pipeline/lzss.h>

#include <stdio.h>

int pipeline_lzss_init(pipeline_ctx_t* ctx, void* more) {
    ctx->state = 0;
    ctx->finish = false;
    ctx->more = more;
    return 0;
}

#define getbit(n, x) { \
    x = 0; \
    for (s = 0; s < n; s++) {  \
        if (mask == 0) { \
            while((buf+l-bufp) < 1) { \
                pipelineReturn(bufp-buf); \
            } \
            b = *bufp; \
            mask = 128; \
            bufp++; \
        } \
        x <<= 1; \
        if (b & mask) x++; \
        mask >>= 1; \
    } \
} \

// This function is not meant to be used in a concurrent way (not thread safe)
// returns < 0 on error, >= the number of bytes used of the buffer
int pipeline_lzss_process(pipeline_ctx_t* ctx, uint8_t* buf, int l) {
    static int i, j, k, r, c, s;
    static unsigned char buffer[N * 2];
    static int b, mask;

    uint8_t* bufp = buf;

    pipelineBegin
    for (i = 0; i < N - F; i++) buffer[i] = ' ';
    mask = 0;
    r = N - F;
    while (1) {
        getbit(1, c);

        if (c) {
            getbit(8, c);
            ctx->next_func->process(ctx->next_ctx, (uint8_t*) &c, 1);
            buffer[r++] = c;
            r &= (N - 1);
        } else {
            getbit(EI, i);
            getbit(EJ, j);

            for (k = 0; k <= j + 1; k++) {
                c = buffer[(i + k) & (N - 1)];
                ctx->next_func->process(ctx->next_ctx, (uint8_t*) &c, 1);
                buffer[r++] = c;  r &= (N - 1);
            }
        }
    }
    pipelineFinish
    ctx->finish = true;
    return bufp-buf;
}

int pipeline_lzss_clear(pipeline_ctx_t*ctx) {
    ctx->next_func->clear(ctx->next_ctx);
    // Do nothing
    return 0;
}
