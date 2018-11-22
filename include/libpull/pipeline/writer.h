/** \file writer.h
 * \author Antonio Langiu
 * \defgroup pip_writer
 * \{
 */
#ifndef LIBPULL_PIPELINE_WRITER_H_
#define LIBPULL_PIPELINE_WRITER_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libpull/pipeline/pipeline.h>

int pipeline_writer_init(pipeline_ctx_t* ctx, void* more);
int pipeline_writer_process(pipeline_ctx_t* ctx, uint8_t* buf, int l);
int pipeline_writer_clear(pipeline_ctx_t*ctx);

static pipeline_func_t writer_pipeline = {
    .init = pipeline_writer_init,
    .process = pipeline_writer_process,
    .clear = pipeline_writer_clear
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBPULL_PIPELINE_WRITER_H_ */
