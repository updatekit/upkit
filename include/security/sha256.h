/** \file sha256.h
 * \brief Set of functions to calculate digest with SHA 256 function.
 * \author Antonio Langiu
 * \defgroup Security
 * \{
 */
#ifndef SHA_256_H_
#define SHA_256_H_

#include <common/libpull.h>
#include "security/digest.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SHA256_INIT(lib) \
    pull_error lib##_sha256_init(digest_ctx *ctx)
#define SHA256_UPDATE(lib) \
    pull_error lib##_sha256_update(digest_ctx *ctx, void *data, size_t data_size)
#define SHA256_FINAL(lib) \
    void * lib##_sha256_final(digest_ctx *ctx)

    /** This struct defines a set of default digest function.
     * You can define your own structure adding the function
     * you need.
     */
#define DIGEST_FUNC(lib) \
    static const digest_func lib##_digest_sha256 = {.size = 32, \
        .init = lib##_sha256_init, \
        .update = lib##_sha256_update, \
        .finalize = lib##_sha256_final}

#ifdef WITH_CRYPTOAUTHLIB
    SHA256_INIT(cryptoauthlib_hw);
    SHA256_UPDATE(cryptoauthlib_hw);
    SHA256_FINAL(cryptoauthlib_hw);

    SHA256_INIT(cryptoauthlib);
    SHA256_UPDATE(cryptoauthlib);
    SHA256_FINAL(cryptoauthlib);
#endif

#ifdef WITH_TINYDTLS
    SHA256_INIT(tinydtls);
    SHA256_UPDATE(tinydtls);
    SHA256_FINAL(tinydtls);
#endif

#ifdef WITH_TINYCRYPT
    SHA256_INIT(tinycrypt);
    SHA256_UPDATE(tinycrypt);
    SHA256_FINAL(tinycrypt);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} SHA_256_H_ */
