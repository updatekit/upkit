#ifndef SHA_256_H_
#define SHA_256_H_

#include "security/digest.h"
#include "common/error.h"

pull_error sha256_init(digest_ctx* ctx);

pull_error sha256_update(digest_ctx* ctx, void* data, size_t data_size);

void* sha256_final(digest_ctx* ctx);

static const digest_func digest_sha256 = 
{ .size = 32, .init = sha256_init, .update = sha256_update, 
    .finalize = sha256_final };

#endif
