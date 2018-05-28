#include <libpull/security/digest.h>
#include <libpull/security/ecc.h>
#include <string.h>

#ifdef WITH_TINYDTLS

#include "tinydtls.h"
#include "crypto.h"

/* SHA 256 */

pull_error tinydtls_sha256_init(digest_ctx* ctx) {
    if (ctx == NULL) {
        return DIGEST_INIT_ERROR;
    }
    dtls_sha256_init(&ctx->sha256_tinydtls.ctx);
    return PULL_SUCCESS;
}

pull_error tinydtls_sha256_update(digest_ctx* ctx, void* data, size_t data_size) {
    if (ctx == NULL || data == NULL) {
        return DIGEST_UPDATE_ERROR;
    }
    dtls_sha256_update(&ctx->sha256_tinydtls.ctx, (unsigned char*)data, data_size);
    return PULL_SUCCESS;
}

void* tinydtls_sha256_final(digest_ctx* ctx) {
    if (ctx == NULL) {
        return NULL;
    }
    dtls_sha256_final((unsigned char*) &ctx->sha256_tinydtls.result, &ctx->sha256_tinydtls.ctx);
    return &ctx->sha256_tinydtls.result;
}

/* ECC */

pull_error tinydtls_secp256r1_ecc_verify(const uint8_t* x, const uint8_t* y, 
        const uint8_t* r, const uint8_t* s, const void* data, uint16_t data_len) {
    int ret = dtls_ecdsa_verify_sig_hash(x, y, 32, data, data_len, (unsigned char*) r, (unsigned char*) s);
    return ret == 0 ? PULL_SUCCESS : VERIFICATION_FAILED_ERROR;
}

pull_error tinydtls_secp256r1_ecc_sign(const uint8_t* private_key, uint8_t *signature,
        const void *data, uint16_t data_len) {
    uint32_t signature_DER[18];
    dtls_ecdsa_create_sig_hash(private_key, 32, data, data_len,
            signature_DER, signature_DER+9);
    // XXX This is not right! tinydtls encodes the signature in DER that requires
    // up to 72 bytes. I'm passing to it just 64. I should refactor the
    // signature to enable different encodings! Use TinyCrypt until this
    // is not solved.
    return SIGN_FAILED_ERROR;
}

#endif /* WITH_TINYDTLS */
