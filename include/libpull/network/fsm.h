#ifndef LIBPULL_FSM_H_
#define LIBPULL_FSM_H_

#include <libpull/common.h>
#include <libpull/network.h>
#include <libpull/memory.h>
#include <libpull/pipeline.h>
#include <libpull/security.h>

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
    safestore_t* sf;
    size_t processed;
    size_t expected;
    mem_id_t id;
    mem_object_t* obj;
    rng_ctx_t rctx;

    mem_id_t old_id;
    mem_object_t* old_obj;
} fsm_ctx_t;

pull_error fsm_init(fsm_ctx_t* ctx, safestore_t* sf, mem_object_t* obj, mem_object_t* old_obj);

pull_error fsm(fsm_ctx_t* ctx, uint8_t* buf, size_t len);

#endif /* LIBPULL_FSM_H_ */
