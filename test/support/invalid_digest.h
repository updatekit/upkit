#ifndef INVALID_DIGEST_H_
#define INVALID_DIGEST_H_

#include "security/digest.h"
#include "common/error.h"

pull_error invalid_init(digest_ctx* ctx) {
    return DIGEST_INIT_ERROR;
}

pull_error invalid_update(digest_ctx* ctx, void* data, size_t data_size) {
    return DIGEST_UPDATE_ERROR;
}

void* invalid_final(digest_ctx* ctx) {
    return NULL;
}

#endif /* INVALID_DIGEST_H_ */
