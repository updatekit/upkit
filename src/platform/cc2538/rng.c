#ifdef WITH_PLATFORM_CC2538

#include <libpull/security.h>

pull_error rng_init(rng_ctx_t* ctx) {
    // I expect the rng to be already initialized during platform initialization
    // https://github.com/RIOT-OS/RIOT/blob/12049a577942fce4b653b30466f12171bdd658ac/sys/auto_init/auto_init.c#L97
    return PULL_SUCCESS;
}

pull_error rng_generate(rng_ctx_t* ctx, nonce_t* nonce) {
    *nonce = random_uint32();
    return PULL_SUCCESS;
}

#endif /* WITH_PLATFORM_CC2538 */
