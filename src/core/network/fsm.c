#include <libpull/common.h>
#include <libpull/memory.h>
#include <libpull/network.h>
#include <libpull/security.h>
#include <libpull/pipeline.h>

#include <string.h>

#define BUFFER_SIZE 1024
uint8_t buffer[BUFFER_SIZE];

pull_error fsm_init(fsm_ctx_t* ctx, safestore_t sf, mem_object_t* obj) {
    ctx->obj = obj;

    // Set the initial state
    ctx->state = STATE_IDLE;
    ctx->processed = 0;

    // Get the current running version
    mem_id_t ignore;
    pull_error err = get_newest_firmware(&ignore, &ctx->version, ctx->obj, true);
    if (err) {
        log_err(err, "Error getting newest firmware\n");
        return err;
    }
    log_debug("Version is %u\n", ctx->version);

    // Init the random number generator
    err = rng_init(&ctx->rctx);
    if (err) {
        log_err(err, "Error initializing RNG\n");
        return err;
    }

    // Load safestore
    ctx->sf = sf;

    // Configure the pipeline
    buffer_pipeline.init(&ctx->buffer_ctx, NULL);
    ctx->buffer_ctx.next_func = &writer_pipeline;
    ctx->buffer_ctx.next_ctx = &ctx->writer_ctx;
    
    return PULL_SUCCESS;
}

pull_error fsm(fsm_ctx_t* ctx, uint8_t* buf, size_t len) {
    PULL_ASSERT(buf != NULL && len > 0);
    pull_error err;
    switch (ctx->state) {
        case STATE_CLEAN:
            /* Clean the stored data */
            memset(&ctx->mt, 0, sizeof(manifest_t));
            ctx->processed = 0;
            ctx->id = 0;
            // TODO if the process already started I should invalidate that object
            ctx->state = STATE_IDLE;
        case STATE_IDLE:
            printf("mamma\n");
            // When the FSM is in state idle the input data must be the new
            // version available. Only if the new version is higher than the
            // current one then the device will move to the next state.
            if (len != sizeof(version_t)) {
                log_err(INVALID_VERSION_ERROR, "Invalid version size\n");
                ctx->state = STATE_CLEAN;
                return INVALID_VERSION_ERROR;
            }
            log_debug("received %x, local %x\n", *((version_t*)buf), ctx->version);
            if ( *((version_t*)buf) <= ctx->version) {
                // If the version is not higher is not a failure. Just there 
                // is no update available
                log_debug("Version is not strictly higher\n");
                return PULL_SUCCESS;
            }
            log_debug("A new version is available\n");
            ctx->state = STATE_START_UPDATE;
            return PULL_SUCCESS;
        case STATE_START_UPDATE:
            if (len < sizeof(receiver_msg_t) || buf == NULL) {
                log_err(INVALID_STATE_ERROR, "Expected buffer to store receiver_msg\n");
                ctx->state = STATE_CLEAN;
                return INVALID_STATE_ERROR;
            }
            receiver_msg_t* msg = (receiver_msg_t*)buf;
            // get nonce

            err = rng_generate(&ctx->rctx, &ctx->nonce);
            if (err) {
                log_err(err, "Error getting RNG\n");
                ctx->state = STATE_CLEAN;
                return err;
            }
            msg->udid = ctx->sf.udid; // set UDID
            msg->version = ctx->version; // set version
            msg->nonce = ctx->nonce;

            ctx->state = STATE_RECEIVE_MANIFEST;
            return PULL_SUCCESS;

        case STATE_RECEIVE_MANIFEST:
            // We expect to receive bytes until the manifest structure is full
            if (ctx->processed > sizeof(manifest_t)) {
                log_err(INVALID_STATE_ERROR, "The number of processed bytes is higher than the manifest size\n");
                ctx->state = STATE_CLEAN;
                return INVALID_STATE_ERROR;
            }
            uint8_t* mtbuf = ((uint8_t*)&ctx->mt) + ctx->processed;
            uint8_t* bufp = buf;
            while (ctx->processed < sizeof(manifest_t) && (bufp-buf) < len) {
                *mtbuf = *bufp;
                ctx->processed++;
                mtbuf++;
                bufp++;
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
            if (get_version(&ctx->mt) <= ctx->version) {
                log_debug("Received version: %u - Local version: %u\n", 
                        get_version(&ctx->mt), ctx->version);
                log_err(INVALID_SIGNATURE_ERROR, "Received firmware has invalid version\n");
                ctx->state = STATE_CLEAN;
                return INVALID_SIGNATURE_ERROR;
            }

            // compare nonce
            if (get_nonce(&ctx->mt) != ctx->nonce) {
                log_err(INVALID_SIGNATURE_ERROR, "Received nonce is invalid\n");                
                ctx->state = STATE_CLEAN;
                return INVALID_SIGNATURE_ERROR;
            }
            err = verify_manifest(&ctx->mt, ctx->sf);
            if (err) {
                log_err(INVALID_SIGNATURE_ERROR,
                        "Manifest has invalid signature\n");
                ctx->state = STATE_CLEAN;
                return INVALID_SIGNATURE_ERROR;
            }
            /* If we are here the verification is successfull and we can fall
             * through to the state receive firmware. We still need to store
             * the manifest and the remaining of the buffer */

            if (get_diff_version(&ctx->mt) == 0) {
                ctx->pipeline = &buffer_pipeline;
                ctx->pipeline_ctx = &ctx->buffer_ctx;
            } else if (get_diff_version(&ctx->mt) == ctx->version) {
                ctx->pipeline = &pipeline_bspatch;
                ctx->pipeline_ctx = &ctx->bsdiff_ctx;
            } else {
                log_err(INVALID_SIGNATURE_ERROR,
                        "Received firmware has invalid diff version\n");
                ctx->state = STATE_CLEAN;
                return INVALID_SIGNATURE_ERROR;
            }

            // Get oldest memory object
            version_t ignore;
            err = get_oldest_firmware(&ctx->id, &ignore, ctx->obj, true);
            if (err) {
                log_err(err, "Error getting the oldest slot");
                ctx->state = STATE_CLEAN;
                return err;
            }
            // Open the memory object
            err = memory_open(ctx->obj, ctx->id, WRITE_ALL);
            if (err) {
                log_err(err, "Error opening the memory_object\n");
                ctx->state = STATE_CLEAN;
                return err;
            }

            // Set the new memory object in the writer pipeline
            writer_pipeline.init(&ctx->writer_ctx, ctx->obj);
            
            // Store the manifest directly without passing through the pipeline
            int ret = buffer_pipeline.process(&ctx->buffer_ctx, (uint8_t*) &ctx->mt, sizeof(manifest_t));
            if (ret != sizeof(manifest_t)) {
                log_err(err, "Error writing manifest");
                ctx->state = STATE_CLEAN;
                return err;
            }
            if ((bufp-buf) < len) {
                ret = ctx->pipeline->process(ctx->pipeline_ctx, bufp, len-(bufp-buf));
                if (ret != len-(bufp-buf)) {
                    log_err(err, "Error writing manifest");
                    ctx->state = STATE_CLEAN;
                    return err;
                } else {
                    ctx->processed += ret; 
                }
            }

            ctx->expected = get_offset(&ctx->mt)+get_size(&ctx->mt);

            ctx->state = STATE_RECEIVE_FIRMWARE;
            return PULL_SUCCESS;
        case STATE_RECEIVE_FIRMWARE:
            if (ctx->processed+len > get_offset(&ctx->mt)+get_size(&ctx->mt)) {
                log_err(INVALID_SIZE_ERROR, "Received more data than expected\n");
                ctx->state = STATE_CLEAN;
                return INVALID_SIZE_ERROR;
            }
            ret = ctx->pipeline->process(ctx->pipeline_ctx, buf, len);
            if (ret != len) {
                log_err(PIPELINE_ERROR, "Error writing manifest");
                ctx->state = STATE_CLEAN;
                return PIPELINE_ERROR;
            }
            ctx->processed += ret;
            if (ctx->processed != ctx->expected) {
                return PULL_SUCCESS;
            }

            ctx->pipeline->clear(ctx->pipeline_ctx);
            memory_close(ctx->obj);
            ctx->state = STATE_VERIFY_FIRMWARE;
            // Fallthrough
        case STATE_VERIFY_FIRMWARE:
            memory_open(ctx->obj, ctx->id, READ_ONLY);
            err = verify_object(ctx->obj, ctx->sf, buffer, BUFFER_SIZE);
            memory_close(ctx->obj);
            
            if (err) {
                log_err(err, "Verification failed\n");
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

