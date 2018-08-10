/** \file pipeline.h
 * \author Antonio Langiu
 * \defgroup pip_pip
 * \{
 */
#ifndef LIBPULL_PIPELINE_PIPELINE_H_
#define LIBPULL_PIPELINE_PIPELINE_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libpull/common.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define pipelineBegin switch(ctx->state) { case 0:
#define pipelineReturn(x) do { ctx->state=__LINE__; return x; \
    case __LINE__:; } while (0)
#define pipelineFinish }

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

typedef struct pipeline_ctx_t pipeline_ctx_t;

typedef struct pipeline_func_t {
    int (*init)(pipeline_ctx_t* ctx, void* more);
    int (*process)(pipeline_ctx_t* ctx, uint8_t* buf, int l);
} pipeline_func_t;

/** 
 * \brief  This structure defines the context for each stage of the pipeline;
 */
typedef struct pipeline_ctx_t {
    int state;                  //< Internal state of the pipeline stage
                                //< (since many stages uses Duff's device).
    void* more;
    bool finish;                //< Indicates whether the process is finished.
    void* stage_data;           //< This value is used internally by the init
                                //< function to store informations for the
                                //< process function.
    pipeline_func_t* next_func; //< Pointer to functions structure of the next
                                //< stage of the pipeline
    pipeline_ctx_t* next_ctx;   //< Pointer to the context to be passed to the
                                //< next stage of the pipeline
} pipeline_ctx_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBPULL_PIPELINE_PIPELINE_H_ */
