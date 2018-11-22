
#ifdef WITH_PLATFORM_POSIX

#include <libpull/security.h>
#include <time.h>
#include <stdlib.h>

pull_error rng_init(rng_ctx_t* ctx) {
    srand(time(NULL));
    return PULL_SUCCESS;
}

pull_error rng_generate(rng_ctx_t* ctx, nonce_t* nonce) {
    // Since nonce is a uint32_t value we can safely cast the
    // output of the rand function
    *nonce = (uint32_t) rand();
    return PULL_SUCCESS;
}

#endif /* WITH_PLATFORM_POSIX */
