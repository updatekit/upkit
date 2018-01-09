/** \file digest.h
 * \brief Set of functions to calculate the digest wrapping some cryptographic
 * libraries.
 * \author Antonio Langiu
 * \defgroup Security
 * \{
 */
#ifndef DIGEST_H_
#define DIGEST_H_

#include "common/error.h"
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

/**
 * Abstraction to use different cryptographic libraries to calculate the hash
 */
typedef struct {
  uint16_t size;                    // Number of bytes of the digest
  pull_error (*init)(digest_ctx *); // Pointer to digest init function
  pull_error (*update)(digest_ctx *, void *,
                       size_t);    // Pointer to digest update function
  void *(*finalize)(digest_ctx *); // Pointer to digest finalize function
} digest_func;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} DIGEST_H_ */
