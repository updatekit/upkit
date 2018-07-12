#include <libpull/fsm/libpull_fsm.h>

#include <libpull/memory.h>

// XXX This must be removed
#include "platform_headers.h"
#include "default_keys.h"
#include <libpull/security.h>

int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}

pull_error libpull_fsm_init(libpull_fsm_ctx_t* ctx, mem_object_t* obj) {
    ctx->state = STATE_IDLE;
    ctx->result = RESULT_INIT;
    ctx->obj = obj;

    ctx->platform = 0xbeef; /* TODO This should be taken from some general structure */

    // (0) Get the newest version available
    mem_id_t ignore;
    pull_error err = get_newest_firmware(&ignore, &ctx->version, ctx->obj, false, false);
    if (err) {
        log_error(err, "Error getting newest firmware\n");
        return err;
    }
    log_debug("Version is %u\n", ctx->version);
    return PULL_SUCCESS;
}

pull_error fsm_pre_verification(manifest_t* mt, void* user_data) {
    /*log_info("Validating received manifest\n");
    libpull_fsm_ctx_t* ctx = (libpull_fsm_ctx_t*) user_data;
    if (get_version(mt) <= ctx->version) {
        log_debug("received version: %u - local version: %u\n", get_version(mt), ctx->version);
        log_error(INVALID_OBJECT_ERROR, "Received firmware has invalid version\n");
        return INVALID_OBJECT_ERROR;
    }
    if (get_platform(mt) != ctx->platform) {
        log_debug("received platform: %u - local platform: %u\n", get_platform(mt), ctx->platform);
        log_error(INVALID_OBJECT_ERROR, "Received firmware has invalid platform\n");
        return INVALID_OBJECT_ERROR;
    }*/
    return PULL_SUCCESS;
}

#define BUFFER_SIZE 1024
uint8_t buffer[BUFFER_SIZE];

pull_error fsm_post_verification(mem_object_t* obj, void* user_data) {
    // XXX These informations should be derived from the specific user data
    return verify_object(obj, tinycrypt_digest_sha256, vendor_x_g, vendor_y_g,
            tinycrypt_secp256r1_ecc, buffer, BUFFER_SIZE);
}

pull_error libpull_fsm_receive(libpull_fsm_ctx_t* ctx, libpull_cmd_t cmd, const uint8_t* buf, size_t len) {
    pull_error err;
    version_t ignore;
    switch (ctx->state) {
        case STATE_IDLE:
            if (cmd != CMD_PACKAGE) {
                break;
            }
            // (0) Get oldest memory object
            err = get_oldest_firmware(&ctx->id, &ignore, ctx->obj, true, true);
            if (err) {
                log_error(err, "Error getting the oldest slot");
                return err;
            }
            // (1) Open the memory object
            err = memory_open(ctx->obj, ctx->id, WRITE_ALL);
            if (err) {
                log_error(err, "Error opening the memory_object\n");
                return err;
            }
            // (2) Open the writer
            err = writer_open(&ctx->wctx, ctx->obj, fsm_pre_verification, ctx);
            if (err) {
                log_error(err, "Error opening the memory_object\n");
                return err;
            }
            ctx->state = STATE_DOWNLOADING;
            if (len == 0 || buf == NULL) {
                break;
            }
            /* fallthrough */
        case STATE_DOWNLOADING:
            if (cmd != CMD_PACKAGE || len == 0 || buf == NULL) {
                return GENERIC_ERROR;
            }
            if ((err = writer_chunk(&ctx->wctx, buf, len)) != PULL_SUCCESS) {
                log_error(err, "Error writing chunk\n");
                ctx->state = STATE_IDLE;
                ctx->result = RESULT_CONNECTION_LOST;
                /* TODO: specialize errors */
                return err;
            }
            ctx->received += len;
            if (!ctx->wctx.manifest_received || /* TODO This can become something like: ctx->wctx.received() */
                (ctx->wctx.manifest_received && (ctx->wctx.received < ctx->wctx.expected))) {
                break;
            }
            log_debug("Changing state to downloaded\n");
            memory_close(ctx->obj);
            ctx->state = STATE_DOWNLOADED;
            break;
        case STATE_DOWNLOADED:
            if (cmd != CMD_UPDATE) {
                break;
            }
            /* received an update command */
            ctx->state = STATE_UPDATING;
            /* fallthrough */
        case STATE_UPDATING:
            // (2) perform verification
            log_debug("Performing verification\n");
            memory_open(ctx->obj, ctx->id, READ_ONLY);
            err = fsm_post_verification(ctx->obj, NULL /* here I must pass some data */);
            memory_close(ctx->obj);
            if (err) {
                log_error(err, "Verification failed\n");
                invalidate_object(ctx->id, ctx->obj);
                ctx->result = RESULT_INTEGRITY_CHECK;
                ctx->state = STATE_IDLE;
                return err;
            }
            log_debug("Verification failed\n");
            ctx->result = RESULT_SUCCESS;
            break;
        default:
            /* TODO manage this error */
            return GENERIC_ERROR;
    }
    return PULL_SUCCESS;
}

