#include "security/digest.h"
#include "security/ecc.h"
#include <string.h>

#ifdef WITH_TINYDTLS

#include "tinydtls.h"
#include "crypto.h"

/* SHA 256 */

pull_error tinydtls_sha256_init(digest_ctx* ctx) {
    dtls_sha256_init(&ctx->sha256_tinydtls.ctx);
    return PULL_SUCCESS;
}

pull_error tinydtls_sha256_update(digest_ctx* ctx, void* data, size_t data_size) {
    dtls_sha256_update(&ctx->sha256_tinydtls.ctx, (unsigned char*)data, data_size);
    return PULL_SUCCESS;
}

void* tinydtls_sha256_final(digest_ctx* ctx) {
    dtls_sha256_final((unsigned char*) &ctx->sha256_tinydtls.result, &ctx->sha256_tinydtls.ctx);
    return &ctx->sha256_tinydtls.result;
}

/* ECC */

pull_error ecc_verify(const uint8_t* x, const uint8_t* y, const uint8_t* r, const uint8_t* s,
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

pull_error ecc_sign(const uint8_t* private_key, const uint8_t *signature,
        const void *data, uint16_t data_len, ecc_curve curve) {
    uint8_t signature_DER[72];
     if(dtls_ecdsa_create_sig_hash(private_key, curve.curve_size, data, data_len,
                 signature_DER, signature_DER+36) == 0) {
         // XXX This is not right! tinydtls encodes the signature in DER that requires
         // up to 72 bytes. I'm passing to it just 64. I should refactor the
         // signature to enable different encodings! Use TinyCrypt until this
         // is not solved.
        return SIGN_FAILED_ERROR
     }
     return SIGN_FAILED_ERROR;
}

#endif /* WITH_TINYDTLS */
