/** \file buffer.h
 * \author Antonio Langiu
 * \defgroup pip_buffer
 * \{
 */
#ifndef LIBPULL_PIPELINE_BUFFER_H_
#define LIBPULL_PIPELINE_BUFFER_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libpull/pipeline/pipeline.h>

// TODO: Fix
#define BUFFER_LEN 1000
struct buffer_ctx {
    uint8_t buffer[BUFFER_LEN];
    uint8_t* bufferp;
};
#undef BUFFER_LEN

static inline void pipeline_buffer_init(pipeline_ctx_t* c, void* more) {
    static struct buffer_ctx ctx;
    ctx.bufferp = ctx.buffer;
    c->stage_data = &ctx;
}

int buffer_pipeline(pipeline_ctx_t* ctx, uint8_t* buf, int l);

int pipeline_buffer_clear(pipeline_ctx_t*ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBPULL_PIPELINE_BUFFER_H_ */
