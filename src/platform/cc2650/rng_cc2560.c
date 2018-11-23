#ifdef WITH_PLATFORM_CC2650

#include <libpull/security.h>
#include "trng.h"

pull_error rng_init(rng_ctx_t* ctx) {
    return PULL_SUCCESS;
}

pull_error rng_generate(rng_ctx_t* ctx, nonce_t* nonce) {
    *nonce = TRNGNumberGet(TRNG_HI_WORD);
    return PULL_SUCCESS;
}

#endif /* WITH_PLATFORM_CC2650 */
