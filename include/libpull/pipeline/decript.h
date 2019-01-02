/** \file decrypt.h
 * \author Antonio Langiu
 * \defgroup pip_decrypt
 * \{
 */
#ifndef LIBPULL_PIPELINE_DECRYPT_H_
#define LIBPULL_PIPELINE_DECRYPT_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libpull/pipeline/pipeline.h>

// TODO: Fix
#define DECRYPT_LEN 1000
struct decrypt_ctx {
    uint8_t decrypt[DECRYPT_LEN];
    uint8_t* decryptp;
};
#undef DECRYPT_LEN

static inline void pipeline_decrypt_init(pipeline_ctx_t* c, void* more) {
    static struct decrypt_ctx ctx;
    ctx.decryptp = ctx.decrypt;
    c->stage_data = &ctx;
}

int decrypt_pipeline(pipeline_ctx_t* ctx, uint8_t* buf, int l);

int pipeline_decrypt_clear(pipeline_ctx_t*ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBPULL_PIPELINE_DECRYPT_H_ */
