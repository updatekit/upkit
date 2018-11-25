#include <libpull/pipeline/buffer.h>
#include <libpull/pipeline/writer.h>
#include <libpull/memory.h>
#include <libpull/common.h>

int writer_pipeline(pipeline_ctx_t* c, uint8_t* buf, int l) {
    struct writer_ctx* ctx = (struct writer_ctx*) c->stage_data;
    int ret = memory_write(ctx->obj, buf, l, ctx->written);
    ctx->written+=ret;
    return ret;
}
