#include <libpull/pipeline/buffer.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_LEN 1000

int pipeline_buffer_init(pipeline_ctx_t* ctx, void* more) {
    // does nothing
    return 0;
}

#define BUFFER_USED (bufp-buffer)
#define MIN(A,B) ((A<B)? A: B)

int pipeline_buffer_process(pipeline_ctx_t* ctx, uint8_t* buf, int l) {
    static uint8_t buffer[BUFFER_LEN]; // This hardfixed value must be configured
    static uint8_t* bufp = buffer;
    int empty = 0, lused = 0;

    while (lused < l) {
        empty = BUFFER_LEN - BUFFER_USED;
        if (empty > 0) {
            int min = MIN(l, empty);
            memcpy(bufp, buf, min);
            lused += min; 
            bufp += min;
        } else if (empty == 0) {
            printf("writing %d bytes\n", BUFFER_USED);
            ctx->next_func->process(ctx->next_ctx, buffer, BUFFER_USED);
            bufp = buffer;
        } else {
            // This is a fatal error since the bufp must never be higher than
            // buffer+sizeof(buffer). This means we are writing over the buffer
            // boundaries.
            return -1;
        }
    }
    return l;
}
