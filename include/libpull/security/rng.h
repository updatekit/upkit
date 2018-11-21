/** \file rng.h
 * \brief A random number generator generates a sequence
 * of numbers that cannot be reasonably predicted better
 * than by a random chance. This interface allows to generate
 * those numbers with different implementations.
 * \author Antonio Langiu
 * \defgroup sec_ecc
 * \{
 */
#ifndef LIBPULL_SECURITY_RNG_H_
#define LIBPULL_SECURITY_RNG_H_

#include <libpull/common.h>
#include <stdint.h>

#ifdef WITH_TINYCRYPT
#include <tinycrypt/hmac_prng.h>
#include <tinycrypt/ctr_prng.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct rng_ctx_t {
#ifdef WITH_TINYCRYPT
    struct tc_hmac_prng_struct ctx;
#endif
} rng_ctx_t;

pull_error rng_init(rng_ctx_t* ctx);

pull_error rng_generate(rng_ctx_t* ctx, nonce_t* nonce);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_SECURITY_RNG_H_ */
