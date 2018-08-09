#ifndef LIBPULL_PIPELINE_PIPELINE_H_
#define LIBPULL_PIPELINE_PIPELINE_H_

#include <libpull/common.h>

#define pipelineBegin switch(ctx->state) { case 0:
#define pipelineReturn(x) do { ctx->state=__LINE__; return x; \
    case __LINE__:; } while (0)
#define pipelineFinish }
typedef struct pipeline_ctx_t pipeline_ctx_t;

typedef struct pipeline_func_t {
    int (*init)(pipeline_ctx_t* ctx, void* more);
    int (*process)(pipeline_ctx_t* ctx, uint8_t* buf, int l);
} pipeline_func_t;

typedef struct pipeline_ctx_t {
    int state;
    void* more;
    bool finish;
    void* stage_data;
    pipeline_func_t* next_func;
    pipeline_ctx_t* next_ctx;
} pipeline_ctx_t;

#endif /* LIBPULL_PIPELINE_PIPELINE_H_ */
