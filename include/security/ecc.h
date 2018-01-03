/** \file ecc.h
 * \brief Set of functions to work with ECC signature.
 * \author Antonio Langiu
 * \defgroup Security
 * \{
 */
#ifndef ECC_VERIFY_H_
#define ECC_VERIFY_H_

#include "common/error.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Enumeration of the supported curves. Some of them may not be supported by
 * the choosed ECC library.*/
typedef enum curve_t {
  CURVE_SECP256R1, /** Default NIST curve. */
  CURVE_SECP224R1
} curve_t;

/** Structure used to describe a curve */
typedef struct ecc_curve {
  curve_t type;       /** Enumeration indicating the curve. */
  uint8_t curve_size; /** Size of the curve. */
} ecc_curve;

/** Default structures describing curves \{ */
static const ecc_curve secp256r1 = {.type = CURVE_SECP256R1, .curve_size = 32};
static const ecc_curve secp224r1 = {.type = CURVE_SECP224R1, .curve_size = 28};
/** \} */

/**
 * \brief  Verify the ECC digital signature.
 *
 * The function verify the signature on some data, normally a hash.
 *
 * \param x The X parameter of the signer's public key.
 * \param y The Y parameter of the signer's public key.
 * \param r The R parameter of the signature.
 * \param s The S parameter of the signature
 * \param data The data on which the signature should be verified
 * \param data_len The lenght of the data.
 * \param curve The ECC curve used to generate the signature.
 *
 * \note The lenght of the signature parameters (X, Y, R, S) is
 * identified by the curve parameter.
 *
 * \returns PULL_SUCCESS is verification succeded or the specific error
 * otherwise.
 */
pull_error ecc_verify(const uint8_t *x, const uint8_t *y, const uint8_t *r,
                      const uint8_t *s, const void *data, uint16_t data_len,
                      ecc_curve curve);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} ECC_VERIFY_H_ */
