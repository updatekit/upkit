/** \file ecc.h
 * \brief Set of functions to work with ECC signature.
 * \author Antonio Langiu
 * \defgroup Security
 * \{
 */
#ifndef LIBPULL_SECURITY_ECC_H_
#define LIBPULL_SECURITY_ECC_H_

#include <libpull/common.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    pull_error (*verify) (const uint8_t *, const uint8_t*, const uint8_t*,
            const uint8_t*, const void*, uint16_t);
    pull_error (*sign) (const uint8_t*, uint8_t*, const void*, uint16_t);
    uint8_t curve_size;
} ecc_func_t;

#define ECC_VERIFY(impl) \
    pull_error impl##_ecc_verify(const uint8_t *pub_x, const uint8_t *pub_y, const uint8_t *r, \
        const uint8_t *s, const void *data, uint16_t data_len)

#define ECC_SIGN(impl) \
    pull_error impl##_ecc_sign(const uint8_t* private_key, uint8_t *signature, \
        const void *data, uint16_t data_len)

#define ECC_FUNC(impl, size) \
    static const ecc_func_t impl##_ecc = { \
        .verify = impl##_ecc_verify, \
        .sign = impl##_ecc_sign, \
        .curve_size = size \
    };

#ifdef WITH_TINYDTLS
    ECC_VERIFY(tinydtls_secp256r1);
    ECC_SIGN(tinydtls_secp256r1);
    ECC_FUNC(tinydtls_secp256r1, 32);
#endif

#ifdef WITH_TINYCRYPT
    ECC_VERIFY(tinycrypt_secp256r1);
    ECC_SIGN(tinycrypt_secp256r1);
    ECC_FUNC(tinycrypt_secp256r1, 32);
#endif

#ifdef WITH_CRYPTOAUTHLIB
    ECC_VERIFY(cryptoauthlib);
    ECC_SIGN(cryptoauthlib);
    ECC_FUNC(cryptoauthlib, 32);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_SECURITY_ECC_H_ */
