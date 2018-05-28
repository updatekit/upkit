#ifndef LIBPULL_COMMON_IDENTITY_H_
#define LIBPULL_COMMON_IDENTITY_H_

/* This struct is often passed by value in the library, so it must
 * not contain any pointer */
typedef struct {
    uint16_t udid;
    uint16_t random;
} identity_t;

pull_error update_random(identity_t* identity);
pull_error validate_identity(identity_t valid_identity, identity_t received_identity);

#endif /* LIBPULL_COMMON_IDENTITY_H_ */
