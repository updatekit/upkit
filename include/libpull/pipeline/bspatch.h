#ifndef LIBPULL_PIPELINE_BSPATCH_DECODER_H_
#define LIBPULL_PIPELINE_BSPATCH_DECODER_H_

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

int pipeline_bspatch_init(pipeline_ctx_t* ctx, void* more);
int pipeline_bspatch_process(pipeline_ctx_t* ctx, uint8_t* buf, int l);

static const pipeline_func_t pipeline_bspatch = {
    .init = pipeline_bspatch_init,
    .process = pipeline_bspatch_process,
};

#endif /* LIBPULL_PIPELINE_BSPATCH_DECODER_H_ */

