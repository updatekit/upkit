/** \file bspatch.h
 * \author Antonio Langiu
 * \defgroup pip_bspatch
 * \{
 */
#ifndef LIBPULL_PIPELINE_BSPATCH_DECODER_H_
#define LIBPULL_PIPELINE_BSPATCH_DECODER_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libpull/pipeline.h>
#include <stdbool.h>

typedef union ctrl_t {
    uint8_t c[8];
    int32_t y;
} ctrl_t;

typedef enum bspatch_state_t {
    READ_HEADER = 0,
    READ_CTRL = 1,
    SUM = 2,
    INSERT = 3,
    SHIFT = 4
} bspatch_state_t;

inline void pipeline_bspatch_init(pipeline_ctx_t* ctx, void* more) {
    ctx->finish = false;
    ctx->more = more;
    ctx->state = 0;
}

int bspatch_pipeline(pipeline_ctx_t* ctx, uint8_t* buf, int l);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBPULL_PIPELINE_BSPATCH_DECODER_H_ */

