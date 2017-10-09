#ifndef ECC_VERIFY_H_
#define ECC_VERIFY_H_

#include <stdint.h>
#include "common/error.h"

typedef enum curve_t{
    CURVE_SECP256R1,
    CURVE_SECP224R1
} curve_t;

typedef struct {
    curve_t type;
    uint8_t curve_size;
} ecc_curve;

static const ecc_curve secp256r1 = { .type = CURVE_SECP256R1, .curve_size = 32 };
static const ecc_curve secp224r1 = { .type = CURVE_SECP224R1, .curve_size = 28 };

pull_error ecc_verify(const uint8_t* x, const uint8_t* y, const uint8_t* r, const uint8_t* s, 
        const void* data, uint16_t data_len, ecc_curve curve);

#endif
