#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/security.h>
#include <string.h>
#include <stdio.h>


/* The memory object should be already opened */
pull_error verify_object(mem_object_t* obj, safestore_t sf, uint8_t* buffer, size_t buffer_len) {
    pull_error err;

    /************* GET_OBJECT_MANIFEST ***************/
    manifest_t mt;
    err = read_firmware_manifest(obj, &mt);
    if (err) {
        return MEMORY_READ_ERROR;
    }
    /************* VERIFY_MANIFEST *******************/
    err = verify_manifest(&mt, sf);
    if (err) {
        return err;
    }
    /************* VERIFY_DIGEST *******************/
    err = verify_digest(obj, &mt, buffer, buffer_len);
    if (err) {
        return err;
    }
    return PULL_SUCCESS;
}

pull_error verify_manifest(manifest_t* mt, safestore_t sf) {
    // Verify App ID
    if (get_appid(mt) != sf.appid) {
        log_err(INVALID_SIGNATURE_ERROR, "Received firmware has invalid appid\n");
        return INVALID_SIGNATURE_ERROR;
    }
    // Verify signature
    pull_error err = verify_signature(mt, sf.keystore);
    if (err) {
        return err;
    }
    log_debug("Valid Signature\n");
    return PULL_SUCCESS;
}

pull_error verify_digest(mem_object_t* obj, manifest_t* mt, uint8_t* buffer,
        size_t buffer_len) {
    pull_error err;
    /************* CALCULATING DIGEST ****************/
    digest_ctx ctx;
    if ((err = digest_init(&ctx)) != PULL_SUCCESS) {
        return err;
    }
    address_t offset = get_offset(mt);
    const address_t final_offset = offset + get_size(mt);
    address_t step = buffer_len;
    log_debug("Digest: initial offset %uu final offset %u size %u\n", offset, final_offset, get_size(mt));
    if (offset == final_offset) {
        return MEMORY_READ_ERROR;
    }
    int readed = 0;
    while (offset < final_offset) {
        if ((readed = memory_read(obj, buffer, step, offset)) != step) {
            log_error(MEMORY_READ_ERROR, "Error reading memory to calculate digest\n");
            return MEMORY_READ_ERROR;
        }
        err = digest_update(&ctx, buffer, readed);
        if (err) {
            return err;
        }
        offset += readed;
        if (offset + step > final_offset) {
            step = final_offset - offset;
        }
    }
    uint8_t* result = digest_finalize(&ctx);
    /***************** VERIFY DIGEST *************/
    const uint8_t* hash = get_digest(mt);
    if (hash == NULL) {
        return INVALID_MANIFEST_ERROR;
    }
    int i=get_digest_size()-1;
    while(i >= 0) {
        if (hash[i] != result[i]) {
            log_debug("Invalid hash\n");
            log_debug("Calculated: %02x %02x %02x %02x\n", result[0], result[1], result[2], result[3]);
            log_debug("Expected: %02x %02x %02x %02x\n", hash[0], hash[1], hash[2], hash[3]);
            return VERIFICATION_FAILED_ERROR;
        }
        i--;
    }
    return PULL_SUCCESS;

}
