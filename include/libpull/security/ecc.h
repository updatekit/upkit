/** \file ecc.h
 * \brief Set of functions to work with ECC signature.
 * \author Antonio Langiu
 * \defgroup sec_ecc
 * \{
 */
#ifndef LIBPULL_SECURITY_ECC_H_
#define LIBPULL_SECURITY_ECC_H_

#include <libpull/common.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

pull_error ecc_verify (const uint8_t *, const uint8_t*, const uint8_t*,
        const uint8_t*, const void*, uint16_t);
pull_error ecc_sign (const uint8_t*, uint8_t*, const void*, uint16_t);
uint8_t ecc_size();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_SECURITY_ECC_H_ */
