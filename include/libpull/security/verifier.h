/** \file verifier.h
 * \author Antonio Langiu
 * \defgroup sec_ver
 * \brief This module includes component to perform secured updates.
 * 
 * The components allow to grant integrity, authenticity and confidentiality
 * during the update process. You can include this components in your update
 * runner to reach your application needs.
 * \{
 */
#ifndef LIBPULL_SECURITY_VERIFIER_H_
#define LIBPULL_SECURITY_VERIFIER_H_

#include <libpull/common.h>
#include <libpull/memory/memory_interface.h>
#include <libpull/security/digest.h>
#include <libpull/security/ecc.h>

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
 * \param[in] obj_t The memory object to validate
 * \param[in] x The X parameter of the signer's public key.
 * \param[in] y The Y parameter of the signer's public key.
 * \param[in] curve The curve parameters.
 * \param[in] buffer The buffer used to read data from the object
 * \param[in] buffer_len The size of the buffer
 *
 * \note The size of the buffer must be greather or equal to the
 * chunk of manifest to be hashed
 *
 * \returns PULL_SUCCESS if verification succeded or the specific error
 * otherwise.
 */
pull_error verify_object(mem_object_t* obj, const uint8_t *x,
                         const uint8_t *y, uint8_t* buffer, size_t buffer_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_SECURITY_VERIFIER_H_ */
