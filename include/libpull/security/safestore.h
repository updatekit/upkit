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

typedef union key_t {
    uint8_t x[32];
    uint8_t y[32];
}

typedef struct safestore_t {
    uint16_t udid;
    platform_t platform;
    key_t vendor_pub_key;
    uint8_t startup_flag;
} safestore_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_SECURITY_SAFESTORE_H_ */
