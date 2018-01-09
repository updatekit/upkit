#include "security/digest.h"
#include "security/sha256.h"
#include "security/ecc.h"
#include <string.h>

#ifdef WITH_TINYDTLS

#include "tinydtls.h"
#include "crypto.h"

/* SHA 256 */

inline pull_error tinydtls_sha256_init(digest_ctx* ctx) {
    dtls_sha256_init(&ctx->sha256_tinydtls.ctx);
    return PULL_SUCCESS;
}

inline pull_error tinydtls_sha256_update(digest_ctx* ctx, void* data, size_t data_size) {
    dtls_sha256_update(&ctx->sha256_tinydtls.ctx, (unsigned char*)data, data_size);
    return PULL_SUCCESS;
}

inline void* tinydtls_sha256_final(digest_ctx* ctx) {
    dtls_sha256_final((unsigned char*) &ctx->sha256_tinydtls.result, &ctx->sha256_tinydtls.ctx);
    return &ctx->sha256_tinydtls.result;
}

/* ECC */

pull_error tinydtls_ecc_verify(const uint8_t* x, const uint8_t* y, const uint8_t* r, const uint8_t* s,
        const void* data, uint16_t data_len, ecc_curve curve) {
    if (curve.type != CURVE_SECP256R1) {
        return NOT_SUPPORTED_CURVE_ERROR;
    }
    if (dtls_ecdsa_verify_sig_hash(x, y, curve.curve_size, 
                data, data_len, (unsigned char*) r, (unsigned char*) s) == 0) {
        return PULL_SUCCESS;
    }
    return VERIFICATION_FAILED_ERROR;
}

#endif /* WITH_TINYDTLS */
