#include "security/digest.h"
#include "security/sha256.h"
#include "security/ecc.h"
#include <string.h>

#ifdef WITH_TINYCRYPT

#include "tinycrypt.h"

/* SHA 256 */

inline pull_error tinycrypt_sha256_init(digest_ctx* ctx) {
    if (tc_sha256_init(&ctx->sha256_tinycrypt.ctx) != 1) {
        return SHA256_INIT_ERROR;
    }
    return PULL_SUCCESS;
}

inline pull_error tinycrypt_sha256_update(digest_ctx* ctx, void* data, size_t data_size) {
    if (tc_sha256_update(&ctx->sha256_tinycrypt.ctx, (const uint8_t *) data, (size_t) data_size) != 1) {
        return SHA256_UPDATE_ERROR;
    }
    return PULL_SUCCESS;
}

inline void* tinycrypt_sha256_final(digest_ctx* ctx) {
    if (tc_sha256_final((uint8_t*) &ctx->sha256_tinycrypt.result, 
                &ctx->sha256_tinycrypt.ctx) != 1) {
        return NULL;
    }
    return &ctx->sha256_tinycrypt.result;
}

/* ECC */

pull_error ecc_verify(const uint8_t* x, const uint8_t* y, const uint8_t* r, const uint8_t* s,
        const void* data, uint16_t data_len, ecc_curve curve) {
    if (curve.type != CURVE_SECP256R1) {
        return NOT_SUPPORTED_CURVE_ERROR;
    }
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

#endif /* WITH_TINYCRYPT */
