/** \file safestore.h
 * \author Antonio Langiu
 * \defgroup sec_ver
 * \brief This module includes component to perform secured updates.
 *
 * The safestore is a structure that must be stored in permantent
 * memory on the device and contains device keys and other permanent data
 * \{
 */
#ifndef LIBPULL_SECURITY_SAFESTORE_H_
#define LIBPULL_SECURITY_SAFESTORE_H_

#include <libpull/common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct keystore_t {
    uint8_t x[32];
    uint8_t y[32];
} keystore_t;

typedef struct safestore_t {
   keystore_t keystore; // contains the vendor public key
   udid_t udid;
   appid_t appid;
   uint8_t aes_key[128];
} safestore_t;

pull_error get_safestore(safestore_t* sf);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_SECURITY_SAFESTORE_H_ */
