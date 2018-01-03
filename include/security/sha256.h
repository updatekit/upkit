/** \file sha256.h
 * \brief Set of functions to calculate digest with SHA 256 function.
 * \author Antonio Langiu
 * \defgroup Security
 * \{
 */
#ifndef SHA_256_H_
#define SHA_256_H_

#include "common/error.h"
#include "security/digest.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** 
 * \brief Init the digest function and context.
 * \param ctx Digest context structure to be initialized.
 * \note This function must be always called before calling update or final.
 * \returns PULL_SUCCESS on success or specific error otherwise.
 */
pull_error sha256_init(digest_ctx *ctx);

/** 
 * \brief Feed the digest with new data. It can be called multiple times.
 * \param ctx The digest context previously initialized.
 * \param data The data to be passed to the algorithm.
 * \param data_size The size of the data.
 * \returns PULL_SUCCESS on success or specific error otherwise.
 */
pull_error sha256_update(digest_ctx *ctx, void *data, size_t data_size);

/** 
 * \brief Finalize the digest and stores it into the context.
 * \param ctx The digest context.
 * \returns PULL_SUCCESS on success or specific error otherwise.
 */
void *sha256_final(digest_ctx *ctx);

/** This static variable identifies this digest algorithm when doing
 * verification */
static const digest_func digest_sha256 = {.size = 32,
                                          .init = sha256_init,
                                          .update = sha256_update,
                                          .finalize = sha256_final};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} SHA_256_H_ */
