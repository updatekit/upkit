#ifndef VERIFIER_H_
#define VERIFIER_H_

#include "common/libpull.h"
#include "memory/memory.h"
#include "memory/memory_objects.h"
#include "memory/manifest.h"
#include "security/verifier.h"
#include "security/digest.h"
#include "security/ecc.h"
#include <string.h>
#include <stdio.h>

enum verifier_states {
    GET_OBJECT_MANIFEST = 0,
    CALCULATING_DIGEST = 1,
    VERIFY_DIGEST = 2,
    VERIFY_VENDOR_SIGNATURE = 3,
    VERIFY_SERVER_SIGNATURE = 4
};

/* The memory object should be already opened */
pull_error verify_object(mem_object* obj, digest_func digest, const uint8_t* x, const uint8_t* y,
        ecc_func_t ef, uint8_t* buffer, size_t buffer_len) {
    PULL_ASSERT(obj == NULL || x == NULL || y == NULL || buffer == NULL || buffer_len == 0);
    pull_error err;
    enum verifier_states state;
    /************* GET_OBJECT_MANIFEST ***************/
    state = GET_OBJECT_MANIFEST;
    manifest_t mt;
    err = read_firmware_manifest(obj, &mt);
    if (err) {
        err = MEMORY_READ_ERROR;
        goto error;
    }
    /************* CALCULATING DIGEST ****************/
    state = CALCULATING_DIGEST;
    digest_ctx ctx;
    if ((err = digest.init(&ctx)) != PULL_SUCCESS) {
        goto error;
    }
    address_t offset = get_offset(&mt);
    address_t final_offset = offset + get_size(&mt);
    address_t step = buffer_len;
    log_debug("Digest: initial offset %lu final offset %lu\n", offset, final_offset);
    if (offset == final_offset) {
        err = MEMORY_READ_ERROR;
        goto error;
    }
    int readed = 0;
    while (offset < final_offset &&
            (readed = memory_read(obj, buffer, step, offset)) > 0) {
        err = digest.update(&ctx, buffer, readed);
        if (err) {
            goto error;
        }
        offset += readed;
        if (offset + step > final_offset) {
            step = final_offset - offset;
        }
    }
    uint8_t* result = digest.finalize(&ctx);
    /***************** VERIFY DIGEST *************/
    state = VERIFY_DIGEST;
    log_debug("Verifying digest\n");
    uint8_t* hash = get_digest(&mt);
    if (hash == NULL) {
        err = INVALID_MANIFEST_ERROR;
        goto error;
    }
    if (memcmp(result, hash, digest.size) != 0) {
        err = VERIFICATION_FAILED_ERROR;
        log_debug("Invalid hash\n");
        goto error;
    }

    /********** VERIFY_VENDOR_SIGNATURE ***********/
    state = VERIFY_VENDOR_SIGNATURE;
    err = verify_manifest_vendor(&mt, digest, x, y, ef);
    if (err) {
        return VERIFICATION_FAILED_ERROR;
    }
    log_debug("Vendor Signature Valid\n");
    /********** VERIFY_SERVER_SIGNATURE ***********/
    state = VERIFY_SERVER_SIGNATURE;
    err = verify_manifest_server(&mt, digest, x, y, ef);
    if (err) {
        return VERIFICATION_FAILED_ERROR;
    }
    log_debug("Server Signature Valid\n");
    return PULL_SUCCESS;
error:
    log_error(err, "Error in the verification process in phase %d\n", state);
    return err;
}

#endif // VERIFIER_H_
