#ifndef LIBPULL_FSM_H_
#define LIBPULL_FSM_H_

#include <libpull/common.h>
#include <libpull/network.h>
#include <libpull/memory.h>
#include <libpull/pipeline.h>

typedef enum fsm_state_t {
    STATE_CLEAN = 0,
    STATE_IDLE = 1,
    STATE_START_UPDATE = 2,
    STATE_RECEIVE_MANIFEST = 3,
    STATE_VERIFY_MANIFEST = 4,
    STATE_RECEIVE_FIRMWARE = 5,
    STATE_VERIFY_FIRMWARE = 6,
    STATE_REBOOT = 7
} fsm_state_t;

typedef struct fsm_ctx_t {
    fsm_state_t state;

    version_t version;
    platform_t platform;
    identity_t identity;

    manifest_t mt;
    size_t processed;

    mem_id_t id;
    mem_object_t* obj;

    pipeline_ctx_t bsdiff_ctx;
    pipeline_ctx_t lzss_ctx;
    pipeline_ctx_t buffer_ctx;

    pipeline_func_t* pipeline;
    pipeline_ctx_t* pipeline_ctx;
} fsm_ctx_t;

pull_error fsm_init(fsm_ctx_t* ctx, mem_object_t* obj);

pull_error fsm(fsm_ctx_t* ctx, const uint8_t* buf, size_t len);

#endif /* LIBPULL_FSM_H_ */
