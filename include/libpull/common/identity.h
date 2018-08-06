/** \file identity.h
 * \brief The identity is used to univocally identify the device.
 * \author Antonio Langiu
 * \defgroup com_ident
 * \{
 */
#ifndef LIBPULL_COMMON_IDENTITY_H_
#define LIBPULL_COMMON_IDENTITY_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* This struct is often passed by value in the library, so it must
 * not contain any pointer */
typedef struct {
    uint16_t udid;
    uint16_t random;
} identity_t;

pull_error update_random(identity_t* identity);
pull_error validate_identity(identity_t valid_identity, identity_t received_identity);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_IDENTITY_H_ */
