/** \file digest.h
 * \brief Set of functions to calculate the digest wrapping some cryptographic
 * libraries.
 * \author Antonio Langiu
 * \defgroup sec_digest
 * \{
 */
#ifndef LIBPULL_SECURITY_DIGEST_H_
#define LIBPULL_SECURITY_DIGEST_H_

#include <libpull/common.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef WITH_TINYDTLS
#include <crypto.h>
#include <tinydtls.h>
#endif

#ifdef WITH_TINYCRYPT
#include <tinycrypt/sha256.h>
#endif

#ifdef WITH_CRYPTOAUTHLIB
#include <cryptoauthlib.h>
#endif

typedef union digest_ctx {
#ifdef WITH_TINYDTLS
  struct sha256_tinydtls_t {
    dtls_sha256_ctx ctx;
    uint8_t result[32];
  } sha256_tinydtls;
#endif
#ifdef WITH_TINYCRYPT
  struct sha256_tinycrypt_t {
    struct tc_sha256_state_struct ctx;
    uint8_t result[32];
  } sha256_tinycrypt;
#endif
#ifdef WITH_CRYPTOAUTHLIB
  struct sha256_cryptoauthlib_hw_t {
    atca_sha256_ctx_t ctx;
    uint8_t result[32];
  } sha256_cryptoauthlib_hw;
  struct sha256_cryptoauthlib_t {
    atcac_sha2_256_ctx ctx;
    uint8_t result[32];
  } sha256_cryptoauthlib;
#endif
} digest_ctx;

pull_error digest_init(digest_ctx* ctx);
pull_error digest_update(digest_ctx* ctx, void* data, size_t size);
uint8_t* digest_finalize(digest_ctx* ctx);
uint8_t digest_size();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} DLIBPULL_SECURITY_IGEST_H_ */
