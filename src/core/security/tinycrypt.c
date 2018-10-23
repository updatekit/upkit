#include <libpull/security/digest.h>
#include <libpull/security/ecc.h>
#include <libpull/common/error.h>
#include <string.h>

#ifdef WITH_TINYCRYPT

#include "tinycrypt.h"

/* SHA 256 */
pull_error digest_init(digest_ctx* ctx) {
    PULL_ASSERT(ctx != NULL);
    if (tc_sha256_init(&ctx->sha256_tinycrypt.ctx) != 1) {
        return DIGEST_INIT_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error digest_update(digest_ctx* ctx, void* data, size_t data_size) {
    PULL_ASSERT(ctx != NULL && data != NULL);
    if (tc_sha256_update(&ctx->sha256_tinycrypt.ctx, (const uint8_t *) data, (size_t) data_size) != 1) {
        return DIGEST_UPDATE_ERROR;
    }
    return PULL_SUCCESS;
}

uint8_t* digest_finalize(digest_ctx* ctx) {
    PULL_ASSERT(ctx != NULL);
    if (tc_sha256_final((uint8_t*) &ctx->sha256_tinycrypt.result, 
                &ctx->sha256_tinycrypt.ctx) != 1) {
        return NULL;
    }
    return &ctx->sha256_tinycrypt.result;
}

inline uint8_t digest_size() {
    return 32;
}

/* ECC */

pull_error ecc_verify(const uint8_t* x, const uint8_t* y, const uint8_t* r, const uint8_t* s,
        const void* data, uint16_t data_len) {
    uint8_t pub_key[64];
    uint8_t signature[64];
    memcpy(pub_key, x, 32);
    memcpy(pub_key+32, y, 32);
    memcpy(signature, r, 32);
    memcpy(signature+32, s, 32);
    if (uECC_verify(pub_key, (uint8_t*) data, (size_t) data_len, signature, uECC_secp256r1()) != 1) {
        return VERIFICATION_FAILED_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error ecc_sign(const uint8_t* private_key, uint8_t *signature, 
                    const void *data, uint16_t data_len) {
     if (uECC_sign(private_key, data, data_len, signature, uECC_secp256r1()) != 1) {
        return SIGN_FAILED_ERROR;
     }
     return PULL_SUCCESS;
}

#endif /* WITH_TINYCRYPT */
