#ifndef VERIFIER_H_
#define VERIFIER_H_

#include "memory/memory.h"
#include "security/ecc.h"
#include "security/digest.h"
#include "security/ecc.h"
#include "security/digest.h"

pull_error verify_object(obj_id id, digest_func f, const uint8_t* x, const uint8_t* y, 
        ecc_curve curve, mem_object* obj_t);

#endif // VERIFIER_H_
