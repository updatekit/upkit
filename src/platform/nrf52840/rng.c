#ifdef WITH_PLATFORM_NRF52840

#include <libpull/security.h>
#include <openthread/platform/random.h>

pull_error rng_init(rng_ctx_t* ctx) {
    return PULL_SUCCESS;
}

pull_error rng_generate(rng_ctx_t* ctx, nonce_t* nonce) {
    *nonce = sys_rand32_get();
    return PULL_SUCCESS;
}

#endif /* WITH_PLATFORM_NRF52840 */
