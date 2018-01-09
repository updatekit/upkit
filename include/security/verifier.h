/** \file verifier.h
 * \brief Set of functions to verify a memory object signature.
 * \author Antonio Langiu
 * \defgroup Security
 * \brief This module includes component to perform secured updates.
 * 
 * The components allow to grant integrity, authenticity and confidentiality
 * during the update process. You can include this components in your update
 * runner to reach your application needs.
 * \{
 */
#ifndef VERIFIER_H_
#define VERIFIER_H_

#include "memory/memory.h"
#include "security/digest.h"
#include "security/ecc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief This function verifies the signature on the object id.
 *
 * The digest function and the curve must match the one used to generate
 * the signature stored into the memory object metadata. The ECC signature
 * does not accept any format to reduce space used to store keys.
 *
 * \param[in] id The id of the memory object.
 * \param[in] f The digest function.
 * \param[in] x The X parameter of the signer's public key.
 * \param[in] y The Y parameter of the signer's public key.
 * \param[in] curve The curve parameters.
 * \param[in] obj_t A temporary memory object.
 * \param[in] buffer The buffer used to read data from the object
 * \param[in] buffer_len The size of the buffer
 *
 * \note The size of the buffer must be greather or equal to the
 * chunk of manifest to be hashed
 *
 * \returns PULL_SUCCESS if verification succeded or the specific error
 * otherwise.
 */
pull_error verify_object(obj_id id, digest_func f, const uint8_t *x,
                         const uint8_t *y, ecc_curve curve, mem_object *obj_t,
                         uint8_t* buffer, size_t buffer_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} VERIFIER_H_ */
