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

/** 
 * \brief  Structure used to univocally identify a device.
 * \note This struct is often passed by value in the library, so it must
 * not contain any pointer.
 */
typedef struct {
    uint16_t udid;   //< Unique device identifier
    uint16_t random; //< Random number generated on the device and passed to the server
} identity_t;

/** 
 * \brief  Function to update the random using a PRNG. The implementation is
 * platform dependent and must often rely on hardware solutions included in the
 * platform since there is no entropy source in many RTOS.
 * 
 * \param identity Pointer to the identity struct containing the value to be
 * updated.
 * 
 * \returns PULL_SUCCESS on success, an error otherwise.
 */
pull_error update_random(identity_t* identity);

/** 
 * \brief Function to validate an identity comparing it to the one stored on the
 * device.
 * 
 * \param valid_identity Pointe to the identity stored on the device.
 * \param received_identity Pointer to the identity received that must be
 * validated.
 * 
 * \returns PULL_SUCCESS if the identity is valid, an error otherwise. 
 */
pull_error validate_identity(identity_t valid_identity, identity_t received_identity);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_IDENTITY_H_ */
