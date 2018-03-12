#include "security/digest.h"
#include "security/ecc.h"
#include "common/error.h"
#include <string.h>

#ifdef WITH_TINYCRYPT

#include "tinycrypt.h"

/* SHA 256 */

pull_error tinycrypt_sha256_init(digest_ctx* ctx) {
    if (tc_sha256_init(&ctx->sha256_tinycrypt.ctx) != 1) {
        return SHA256_INIT_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error tinycrypt_sha256_update(digest_ctx* ctx, void* data, size_t data_size) {
    if (tc_sha256_update(&ctx->sha256_tinycrypt.ctx, (const uint8_t *) data, (size_t) data_size) != 1) {
        return SHA256_UPDATE_ERROR;
    }
    return PULL_SUCCESS;
}

void* tinycrypt_sha256_final(digest_ctx* ctx) {
    if (tc_sha256_final((uint8_t*) &ctx->sha256_tinycrypt.result, 
                &ctx->sha256_tinycrypt.ctx) != 1) {
        return NULL;
    }
    return &ctx->sha256_tinycrypt.result;
}

/* ECC */

pull_error ecc_verify(const uint8_t* x, const uint8_t* y, const uint8_t* r, const uint8_t* s,
        const void* data, uint16_t data_len, ecc_curve curve) {
    if (curve.type != CURVE_SECP256R1) {
        return NOT_SUPPORTED_CURVE_ERROR;
    }
    uint8_t pub_key[64];
    uint8_t signature[64];
    memcpy(pub_key, x, 32);
    memcpy(pub_key+32, y, 32);
    memcpy(signature, r, 32);
    memcpy(signature+32, s, 32);
    if (uECC_verify(pub_key, (uint8_t*) data, (size_t) data_len, signature, uECC_secp256r1()) != 1) { // XXX hardcoded curve
        return VERIFICATION_FAILED_ERROR;

    }
    return PULL_SUCCESS;
}

pull_error ecc_sign(const uint8_t* private_key, uint8_t *signature, 
                    const void *data, uint16_t data_len, ecc_curve curve) {
     if (uECC_sign(private_key, data, data_len, signature, uECC_secp256r1()) != 1) { // XXX hardcoded curve
        return SIGN_FAILED_ERROR;
     }
     return PULL_SUCCESS;
}


#if defined(unix) || defined(__linux__) || defined(__unix__) || \
    defined(__unix) |  (defined(__APPLE__) && defined(__MACH__)) || \
    defined(uECC_POSIX)

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif

#warning "using default_CSPRING";

int default_CSPRNG(uint8_t *dest, unsigned int size) {

    /* input sanity check: */
    if (dest == (uint8_t *) 0 || (size <= 0))
        return 0;

    int fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        fd = open("/dev/random", O_RDONLY | O_CLOEXEC);
        if (fd == -1) {
            return 0;
        }
    }

    char *ptr = (char *)dest;
    size_t left = (size_t) size;
    while (left > 0) {
        ssize_t bytes_read = read(fd, ptr, left);
        if (bytes_read <= 0) { // read failed
            close(fd);
            return 0;
        }
        left -= bytes_read;
        ptr += bytes_read;
    }

    close(fd);
    return 1;
}

#endif /* platform */

#endif /* WITH_TINYCRYPT */
