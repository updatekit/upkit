#include "security/digest.h"
#include "security/ecc.h"
#include <string.h>
#include <stdio.h>

#ifdef WITH_CRYPTOAUTHLIB

#include "cryptoauthlib.h"

/* SHA 256 */

/* Hardware */

inline pull_error cryptoauthlib_hw_sha256_init(digest_ctx* ctx) {
    if (atcab_hw_sha2_256_init(&ctx->sha256_cryptoauthlib_hw.ctx) != ATCA_SUCCESS) {
        return SHA256_INIT_ERROR;
    }
    return PULL_SUCCESS;
}

inline pull_error cryptoauthlib_hw_sha256_update(digest_ctx* ctx, void* data, size_t data_size) {
    if (atcab_hw_sha2_256_update(&ctx->sha256_cryptoauthlib_hw.ctx, (const uint8_t *) data, data_size) != ATCA_SUCCESS) {
        return SHA256_UPDATE_ERROR;
    }
    return PULL_SUCCESS;
}

inline void* cryptoauthlib_hw_sha256_final(digest_ctx* ctx) {
    if (atcab_hw_sha2_256_finish(&ctx->sha256_cryptoauthlib_hw.ctx, (uint8_t*) &ctx->sha256_cryptoauthlib.result) != ATCA_SUCCESS) {
        return NULL;
    }
    return &ctx->sha256_cryptoauthlib.result;
}

/* Software */

inline pull_error cryptoauthlib_sha256_init(digest_ctx* ctx) {
    if (atcac_sw_sha2_256_init(&ctx->sha256_cryptoauthlib.ctx) != ATCA_SUCCESS) {
        return SHA256_INIT_ERROR;
    }
    return PULL_SUCCESS;
}

inline pull_error cryptoauthlib_sha256_update(digest_ctx* ctx, void* data, size_t data_size) {
    if (atcac_sw_sha2_256_update(&ctx->sha256_cryptoauthlib.ctx, (const uint8_t *) data, data_size) != ATCA_SUCCESS) {
        return SHA256_UPDATE_ERROR;
    }
    return PULL_SUCCESS;
}

inline void* cryptoauthlib_sha256_final(digest_ctx* ctx) {
    if (atcac_sw_sha2_256_finish(&ctx->sha256_cryptoauthlib.ctx, (uint8_t*) &ctx->sha256_cryptoauthlib.result) != ATCA_SUCCESS) {
        return NULL;
    }
    return &ctx->sha256_cryptoauthlib.result;
}
/* ECC */

pull_error cryptoauthlib_ecc_verify(const uint8_t* x, const uint8_t* y, const uint8_t* r, const uint8_t* s,
        const void* data, uint16_t data_len, ecc_curve curve) {
    uint8_t pub_key[64];
    uint8_t signature[64];
    bool is_verified;
    if (curve.type != CURVE_SECP256R1) {
        return NOT_SUPPORTED_CURVE_ERROR;
    }
    if (data_len != 32) {
        return VERIFICATION_FAILED_ERROR;
    }
    memcpy(pub_key, x, 32);
    memcpy(pub_key+32, y, 32);
    memcpy(signature, r, 32);
    memcpy(signature+32, s, 32);
    if (atcab_verify_extern(data, (const uint8_t*)signature, (const uint8_t*)pub_key, &is_verified)
            != ATCA_SUCCESS) {
        return VERIFICATION_FAILED_ERROR;
    }
    return is_verified? PULL_SUCCESS: VERIFICATION_FAILED_ERROR;
}

#endif /* WITH_TINYCRYPT */
