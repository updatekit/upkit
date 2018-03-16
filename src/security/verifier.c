#ifndef VERIFIER_H_
#define VERIFIER_H_

#include "memory/memory.h"
#include "memory/manifest.h"
#include "security/verifier.h"
#include "security/digest.h"
#include "security/ecc.h"
#include "common/error.h"
#include "common/logger.h"
#include <string.h>
#include <stdio.h>

pull_error verify_object(obj_id id, digest_func digest, const uint8_t* x, const uint8_t* y,
        ecc_func_t ef, mem_object* obj_t, uint8_t* buffer, size_t buffer_len) {
    pull_error err;
    err = memory_open(obj_t, id);
    if (err) {
        log_error(err, "Error opening object %d\n", id);
        goto error;
    }
    // XXX this is very bad, but to fix it I should refactor the memory objects
    // to have the possibility to work on a already open object
    manifest_t mt;
    if (memory_read(obj_t, (uint8_t*) &mt, sizeof(manifest_t), 0) != sizeof(manifest_t)) {
        err = MEMORY_READ_ERROR;
        log_error(err, "Failure reading manifest\n");
        goto error;
    }

    log_info("Calculating digest\n");
    digest_ctx ctx;
    err = digest.init(&ctx);
    if (err) {
        log_error(err, "Error initializing digest\n");
        goto error;
    }
    address_t offset = get_offset(&mt);
    address_t final_offset = offset + get_size(&mt);
    address_t step = buffer_len;
    if (offset == final_offset) {
        err = MEMORY_READ_ERROR;
        log_error(err, "Invalid manifest\n");
        goto error;
    }

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
    uint8_t* hash = get_digest(&mt);
    if (hash == NULL) {
        err = VERIFICATION_FAILED_ERROR;
        log_error(err, "Null hash");
        goto error;
    }
    if (memcmp(result, hash, digest.size) != 0) {
        err = VERIFICATION_FAILED_ERROR;
        log_error(err, "Invalid hash");
        goto error;
    }

    err = verify_manifest_vendor(&mt, digest, x, y, ef);
    if (err) {
        return VERIFICATION_FAILED_ERROR;
    }
    log_info("Vendor Signature Valid");
    // XXX server and vendor are using the same key
    err = verify_manifest_server(&mt, digest, x, y, ef);
    if (err) {
        return VERIFICATION_FAILED_ERROR;
    }
    log_info("Server Signature Valid");
    err = PULL_SUCCESS;
error:
    memory_close(obj_t);
    return err;
}

#endif // VERIFIER_H_
