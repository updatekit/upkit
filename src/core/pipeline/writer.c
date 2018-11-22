#include <libpull/pipeline/buffer.h>
#include <libpull/memory.h>
#include <libpull/common.h>

struct writer_ctx {
    mem_object_t* obj;
    address_t written;
};

int pipeline_writer_init(pipeline_ctx_t* c, void* more) {
    static struct writer_ctx ctx;
    ctx.obj = (mem_object_t*)more;
    ctx.written = 0;
    c->stage_data = &ctx;
    return 0;
}
int pipeline_writer_process(pipeline_ctx_t* c, uint8_t* buf, int l) {
    struct writer_ctx* ctx = (struct writer_ctx*) c->stage_data;
    int ret = memory_write(ctx->obj, buf, l, ctx->written);
    ctx->written+=ret;
    return ret;
}
int pipeline_writer_clear(pipeline_ctx_t*ctx) {
    return 0;
}
