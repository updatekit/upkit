#ifndef VERIFIER_H_
#define VERIFIER_H_

#include "memory/memory.h"
#include "memory/metadata.h"
#include "security/verifier.h"
#include "security/sha256.h"
#include "security/ecc.h"
#include "common/error.h"
#include "common/logger.h"
#include <string.h>
#include <stdio.h>

pull_error verify_object(obj_id id, digest_func digest, const uint8_t* x, const uint8_t* y,
        ecc_curve curve, mem_object* obj_t, uint8_t* buffer, size_t buffer_len) {
    pull_error err;
    err = memory_open(obj_t, id);
    if (err) {
        log_error(err, "Error opening object %d\n", id);
        goto error;
    }
    // XXX this is very bad, but to fix it I should refactor the memory objects
    // to have the possibility to work on a already open object
    metadata mt;
    if (memory_read(obj_t, (uint8_t*) &mt, sizeof(metadata), 0) != sizeof(metadata)) {
        log_error(MEMORY_READ_ERROR, "Failure reading metadata\n");
        err = MEMORY_READ_ERROR;
        goto error;
    }
    log_info("Calculating digest\n");
    digest_ctx ctx;
    err = digest.init(&ctx);
    if (err) {
        log_error(err, "Error initializing digest\n");
        goto error;
    }
    int chunk_len = to_digest(&mt, buffer, buffer_len);
    if (chunk_len < 0) {
        err = DIGEST_UPDATE_ERROR;
        log_error(err, "Failure passing metadata to digest\n");
        goto error;
    }
    err = digest.update(&ctx, buffer, chunk_len);
    if (err) {
        log_error(err, "Failure updating digest\n");
        goto error;
    }
    address_t offset = get_offset(&mt);
    address_t final_offset = offset + get_size(&mt);
    address_t step = buffer_len;
    int readed = 0;
    while (offset < final_offset &&
            (readed = memory_read(obj_t, buffer, step, offset)) > 0) {
        err = digest.update(&ctx, buffer, readed);
        if (err) {
            log_error(err, "Digest update\n");
            goto error;
        }
        offset += readed;
        if (offset + step > final_offset) {
            step = final_offset - offset;
        }
    }
    uint8_t* result = digest.finalize(&ctx);
    log_debug("Digest: %02x %02x\n", result[0], result[1]);
    err = ecc_verify(x, y, mt.vendor_signature_r, mt.vendor_signature_s, 
            result, digest.size, curve);
    if (err) {
        goto error;
    }
    err = PULL_SUCCESS;
error:
    memory_close(obj_t);
    return err;
}

#endif // VERIFIER_H_
