#include <libpull/pipeline/lzss.h>
#include <stdio.h>

#define EI 10  /* typically 10..13 */
#define EJ  4  /* typically 4..5 */
#define P   1  /* If match length <= P then output one character */
#define N (1 << EI)  /* buffer size */
#define F ((1 << EJ) + 1)  /* lookahead buffer size */

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
int lzss_pipeline(pipeline_ctx_t* ctx, uint8_t* buf, int l) {
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
            if (ctx->next_stage(ctx->next_ctx, (uint8_t*) &c, 1) < 0) {
                return -1;
            }
            buffer[r++] = c;
            r &= (N - 1);
        } else {
            getbit(EI, i);
            getbit(EJ, j);

            for (k = 0; k <= j + 1; k++) {
                c = buffer[(i + k) & (N - 1)];
                ctx->next_stage(ctx->next_ctx, (uint8_t*) &c, 1);
                buffer[r++] = c;  r &= (N - 1);
            }
        }
    }
    pipelineFinish
    ctx->finish = true;
    return bufp-buf;
}
