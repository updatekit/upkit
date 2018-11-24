#include <libpull/security/digest.h>
#include <libpull/security/ecc.h>
#include <libpull/common/error.h>
#include <string.h>

#if WITH_MBEDTLS

#include <mbedtls/sha256.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/ctr_drbg.h>

/* SHA 256 */

pull_error digest_init(digest_ctx* ctx) {
    mbedtls_sha256_starts_ret(&ctx->sha256_mbedtls.ctx, 0);
    return PULL_SUCCESS;
}

pull_error digest_update(digest_ctx* ctx, void* data, size_t data_size) {
    if (ctx == NULL || data == NULL || 
        mbedtls_sha256_update_ret(&ctx->sha256_mbedtls.ctx, (const uint8_t *) data, (size_t) data_size) != 0) {
        return DIGEST_UPDATE_ERROR;
    }
    return PULL_SUCCESS;
}

void* digest_finalize(digest_ctx* ctx) {
    if (ctx == NULL || mbedtls_sha256_finish_ret(&ctx->sha256_mbedtls.ctx, 
                (uint8_t*) &ctx->sha256_mbedtls.result) != 0) {
        return NULL;
    }
    return &ctx->sha256_mbedtls.result;
}

uint16_t get_digest_size() {
    return 32;
}

/* ECC */

pull_error ecc_verify(const uint8_t* x, const uint8_t* y, const uint8_t* r, const uint8_t* s,
        const void* data, uint16_t data_len) {
    int ret = 0;
    mbedtls_ecdsa_context ctx_verify;
    mbedtls_ecdsa_init( &ctx_verify );

    uint8_t pub_key_buff[64];
    memcpy(pub_key_buff, x, 32);
    memcpy(pub_key_buff+32, y, 32);

    if (mbedtls_ecp_group_load(&ctx_verify.grp, MBEDTLS_ECP_DP_SECP256R1)) {
        log_debug("Error loading group\n");
        return VERIFICATION_FAILED_ERROR;
    }

    ret = mbedtls_ecp_point_read_binary( &ctx_verify.grp, &ctx_verify.Q,
                           pub_key_buff, 64);
    if (ret != 0) {
        log_err(VERIFICATION_FAILED_ERROR, "Error copying public key\n");
        return VERIFICATION_FAILED_ERROR;
    }

    uint8_t signature[64];
    memcpy(signature, r, 32);
    memcpy(signature+32, s, 32);

    if( ( ret = mbedtls_ecdsa_read_signature( &ctx_verify,
                    data, data_len,
                    (const unsigned char *) &signature, 64 ) ) != 0 ) {
        return VERIFICATION_FAILED_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error ecc_sign(const uint8_t* private_key, uint8_t *signature, 
        const void *data, uint16_t data_len) {
    int ret = 0;
    mbedtls_ecdsa_context ctx_sign;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ecdsa_init(&ctx_sign);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    size_t signature_len = 64;

    if((ret = mbedtls_ecdsa_write_signature( &ctx_sign, MBEDTLS_MD_SHA256,
                    data, data_len, signature, &signature_len,
                    mbedtls_ctr_drbg_random, &ctr_drbg)) != 0 ) {
        return SIGN_FAILED_ERROR;
    }
    return PULL_SUCCESS;
}

uint8_t get_curve_size() {
    return 32; 
}

#endif /* WITH_MBEDTLS */
