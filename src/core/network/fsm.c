#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/network.h>
#include <libpull/security.h>
#include <libpull/pipeline.h>

#define BUFFER_SIZE 1024
uint8_t buffer[BUFFER_SIZE];

pull_error fsm_init(fsm_ctx_t* ctx, mem_obj_t* obj) {
    ctx->obj = obj;

    /* TODO This should be taken from some general structure */
    ctx->application = 0xabcd;
    ctx->platform = 0xbeef;

    ctx->pipeline_diff = pip_diff;
    ctx->pipeline_full = pip_full;

    mem_id_t ignore;
    pull_error err = get_newest_firmware(&ignore, &ctx->version, ctx->obj, true);
    if (err) {
        log_error(err, "Error getting newest firmware\n");
        return err;
    }
    log_debug("Version is %u\n", ctx->version);
    return PULL_SUCCESS;
}

pull_error fsm(fsm_ctx_t* ctx, const uint8_t* buf, size_t len) {
    PULL_ASSERT(buf != NULL && len > 0);
    pull_error err;
    switch (ctx->state) {
        case STATE_CLEAN:
            /* Clean the stored data */
            memset(ctx->mt, 0, sizeof(manifest_t));
            ctx->processed = 0;
            ctx->id = 0;
            // TODO if the process already started I should invalidate that object
            ctx->state = STATE_IDLE;
        case STATE_IDLE:
            // When the FSM is in state idle the input data must be the new
            // version available. Only if the new version is higher than the
            // current one then the device will move to the next state.
            if (len != sizeof(version_t)) {
                log_error(ERROR_INVALID_VERSION, "Invalid version size\n");
                ctx->state = STATE_CLEAN;
                return ERROR_INVALID_VERSION;
            }
            if ( *((version_t*)buf) < ctx->version) {
                // If the version is not higher is not a failure. Just there 
                // is no update available
                log_debug("Version is not strictly higher\n");
                return PULL_SUCCESS;
            }
            log_debug("A new version is available\n");

            // If all the previous checks are valid, fallthough
            // and generate the nonce and the identity for the request
            ctx->state = STATE_START_UPDATE;
            return PULL_SUCCESS;
        case STATE_START_UPDATE:
            if (len < sizeof(reciver_msg) || buf == NULL) {
                log_err(ERROR_INVALID_STATE, "Expected buffer to store receiver_msg\n");
                ctx->state = STATE_CLEAN;
                return ERROR_INVALID_STATE;
            }
            receiver_msg* msg = (receiver_msg*)buf;
            // get nonce
            err = get_nonce(ctx->nonce);
            if (err) {
                log_err(err, "Error getting nonce\n");
                ctx->state = STATE_CLEAN;
                return err;
            }

            msg->id = ctx->identity; // set UDID
            msg->version = ctx->version; // set version

            ctx->state = STATE_RECEIVE_MANIFEST;
            return PULL_SUCCESS;
        case STATE_RECEIVE_MANIFEST:
            // We expect to receive bytes until the manifest structure is full
            if (ctx->processed > sizeof(manifest_t)) {
                log_err(ERROR_INVALID_STATE, "The number of processed bytes is higher than the manifest size\n");
                ctx->state = STATE_CLEAN;
                return err;
            }
            uint8_t* mtbuf = &ctx->mt[ctx->processed];
            uint8_t* bufp = buf;
            while (ctx->processed < sizeof(manifest_t) && (bufp-buf) < len) {
                mtbuf++ = bufp++;
                ctx->processed++;
            }
            if (ctx->processed < sizeof(manifest_t)) {
                log_debug("More data are required to fill the manifest\n");
                return PULL_SUCCESS;
            }
            // If we are here it means that the manifest structure has been
            // filled, howver we may still have more data in the buffer
            // Fallthrough to verify. If it is invalid we do not need to open
            // the memory and store the data
        case STATE_VERIFY_MANIFEST:
            if (get_version(ctx->mt) <= ctx->version) {
                log_debug("Received version: %u - Local version: %u\n", 
                        get_version(ctx->mt), ctx->version);
                log_error(INVALID_SIGNATURE_ERROR, "Received firmware has invalid version\n");
                ctx->state = STATE_CLEAN;
                return INVALID_SIGNATURE_ERROR;
            }
            if (get_platform(ctx->mt) != ctx->platform) {
                log_debug("received platform: %u - local platform: %u\n", 
                        get_platform(ctx->mt), ctx->platform);
                log_error(INVALID_SIGNATURE_ERROR, "Received firmware has invalid platform\n");
                ctx->state = STATE_CLEAN;
                return INVALID_SIGNATURE_ERROR;
            }
            // TODO: compare nonce
            // TODO: compare ID
            err = verify_signature(&mt);
            if (err) {
                log_error(INVALID_SIGNATURE_ERROR,
                        "Received firmware has invalid platform\n");
                ctx->state = STATE_CLEAN;
                return INVALID_SIGNATURE_ERROR;
            }
            /* If we are here the verification is successfull and we can fall
             * through to the state receive firmware. We still need to store
             * the manifest and the remaining of the buffer */

            if (ctx->mt->msg.version == 0) {
                ctx->pipeline = buffer_pipeline;
                ctx->pipeline_ctx = &ctx->buffer_ctx;
            } else {
                ctx->pipeline = bsdiff_pipeline;
                ctx->pipeline_ctx = &ctx->bsdiff_ctx;
            }

            // Get oldest memory object
            version_t ignore;
            err = get_oldest_firmware(&ctx->id, &ignore, ctx->obj, true);
            if (err) {
                log_error(err, "Error getting the oldest slot");
                ctx->state = STATE_CLEAN;
                return err;
            }
            // Open the memory object
            err = memory_open(ctx->obj, ctx->id, WRITE_ALL);
            if (err) {
                log_error(err, "Error opening the memory_object\n");
                ctx->state = STATE_CLEAN;
                return err;
            }
            
            int ret = buffer_pipeline.process(ctx->buffer_ctx, ctx->mt, sizeof(manifest_t));
            if (ret != sizeof(manifest_t)) {
                log_error("Error writing manifest");
                ctx->state = STATE_CLEAN;
                return err;
            }
            if ((bufp-buf) < len) {
                ret = ctx->pipeline.process(&ctx->pipeline_ctx, bufp, len-(bufp-buf));
                if (ret != len-(bufp-buf)) {
                    log_error("Error writing manifest");
                    ctx->state = STATE_CLEAN;
                    return err;
                } else {
                    ctx->processed += ret; 
                }
            }
            ctx->state = STATE_RECEIVE_FIRMWARE;
            return PULL_SUCCESS;
        case STATE_RECEIVE_FIRMWARE:
            if (ctx->processed+len > get_size(cts->mt)) {
                log_error(INVALID_SIZE_ERROR, "Received more data than expected\n");
                ctx->state = STATE_CLEAN;
                return err;
            }
            ret = ctx->pipeline.process(&ctx->pipeline_ctx, buf, len);
            if (ret != len) {
                log_error("Error writing manifest");
                ctx->state = STATE_CLEAN;
                return err;
            }
            ctx->processed += ret;
            if (ctx->processed != ctx->expected) {
                return PULL_SUCCESS;
            }
            // if all the data has been received fall through to state firmware
            // verify
            ctx->state = STATE_VERIFY_FIRMWARE;
        case STATE_VERIFY_FIRMWARE:
            memory_close(ctx->obj);

            memory_open(ctx->obj, ctx->id, READ_ONLY);
            err = verify_object(obj, tinycrypt_digest_sha256, vendor_x_g, vendor_y_g,
                    tinycrypt_secp256r1_ecc, buffer, BUFFER_SIZE);
            memory_close(ctx->obj);
            
            if (err) {
                log_error(err, "Verification failed\n");
                invalidate_object(ctx->id, ctx->obj);
                ctx->state = STATE_IDLE;
                return err;
            }
            ctx->state = STATE_REBOOT;
        case STATE_REBOOT:
            return PULL_SUCCESS;
    }
    return PULL_SUCCESS;
}

