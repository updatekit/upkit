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

int pipeline_buffer_init(pipeline_ctx_t* ctx, void* more);
int pipeline_buffer_process(pipeline_ctx_t* ctx, uint8_t* buf, int l);

static const pipeline_func_t buffer_pipeline = {
    .init = pipeline_buffer_init,
    .process = pipeline_buffer_process,
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBPULL_PIPELINE_BUFFER_H_ */
