/** \file aes.h
 * \brief Interface for decrypt functions
 * libraries.
 * \author Antonio Langiu
 * \defgroup sec_aes
 * \{
 */
#ifndef LIBPULL_SECURITY_AES_H_
#define LIBPULL_SECURITY_AES_H_

#include <libpull/common.h>
#include <libpull/security/safestore.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if WITH_TINYDTLS
#include <aes/rijndael.h>
#endif

#if WITH_TINYCRYPT
#include <tinycrypt/aes.h>
#endif

#if WITH_MBEDTLS
#endif

typedef union aes128_ctx_t {
#if WITH_TINYDTLS
  struct aes128_tinydtls {
      rijndael_ctx ctx;
  } aes128_tinydtls;
#endif
#if WITH_TINYCRYPT
  struct aes128_tinycrypt {
      struct tc_aes_key_sched_struct s;
  } aes128_tinycrypt;
#endif
#if WITH_CRYPTOAUTHLIB
#endif
#if WITH_MBEDTLS
#endif
} aes128_ctx_t;

pull_error aes128_init(aes128_ctx_t *, safestore_t* sf);
pull_error aes128_decypt(aes128_ctx_t *, uint8_t* out, const uint8_t* in);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} DLIBPULL_SECURITY_AES_H_ */
