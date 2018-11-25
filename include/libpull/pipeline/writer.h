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

struct writer_ctx {
    mem_object_t* obj;
    address_t written;
};

static inline void pipeline_writer_init(pipeline_ctx_t* c, void* more) {
    static struct writer_ctx ctx;
    ctx.obj = (mem_object_t*)more;
    ctx.written = 0;
    c->stage_data = &ctx;    
}
int writer_pipeline(pipeline_ctx_t* ctx, uint8_t* buf, int l);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBPULL_PIPELINE_WRITER_H_ */
