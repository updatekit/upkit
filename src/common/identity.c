#include "common/libpull.h"
#include <string.h>

pull_error update_random(identity_t* identity) {
    /*if (identity == NULL) {
        return GENERIC_ERROR;
    }
    pull_error err = get_random_number(identity->random);*/
    return NOT_IMPLEMENTED_ERROR;
}

pull_error validate_identity(identity_t valid_identity, identity_t received_identity) {
    log_debug("valid: %04x %04x received %04x %04x\n", valid_identity.udid, valid_identity.random,
            received_identity.udid, received_identity.random);
    return (memcmp(&valid_identity, &received_identity, sizeof(identity_t))==0)? PULL_SUCCESS : GENERIC_ERROR;
}
