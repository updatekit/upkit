#ifndef DIGEST_H_
#define DIGEST_H_

#include "common/error.h"
#include <stdint.h>
#include <stddef.h>

#ifdef WITH_TINYDTLS
#include <tinydtls.h>
#include <crypto.h>
#endif

#ifdef WITH_TINYCRYPT
#include <tinycrypt/sha256.h>
#endif

typedef union digest_ctx {
#ifdef WITH_TINYDTLS
    struct sha256_tinydtls_t {
        dtls_sha256_ctx ctx;
        uint8_t result[32];
    } sha256_tinydtls;
#endif
#ifdef WITH_TINYCRYPT
    struct sha256_tinycrypt_t {
        struct tc_sha256_state_struct ctx;
        uint8_t result[32];
    } sha256_tinycrypt;
#endif
} digest_ctx;

typedef struct {
    uint16_t size;
    pull_error (*init) (digest_ctx*);
    pull_error (*update) (digest_ctx*, void*, size_t);
    void* (*finalize) (digest_ctx*);
} digest_func;

#endif // DIGEST_H_
