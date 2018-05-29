#ifndef AGENTS_UPDATE_H_
#define AGENTS_UPDATE_H_

#include <libpull/common.h>
#include <libpull/network.h>
#include <libpull/memory.h>
#include <libpull/security.h>

#include <libpull_agents/coroutines.h>

#include "platform_headers.h"

#define IS_CONTINUE(agent_msg) (agent_msg.event > EVENT_CONTINUE_START_ && agent_msg.event < EVENT_CONTINUE_END_ )
#define IS_SEND(agent_msg) (agent_msg.event > EVENT_SEND_START_ && agent_msg.event < EVENT_SEND_END_ )
#define IS_RECOVER(agent_msg) (agent_msg.event > EVENT_RECOVER_START_ && agent_msg.event < EVENT_SEND_END_ )
#define IS_FAILURE(agent_msg) (agent_msg.event > EVENT_FAILURE_START_ && agent_msg.event < EVENT_FAILURE_END_ )

#define GET_CONNECTION(agent_msg) ((txp_ctx*) (agent_msg.event_data))
#define GET_ERROR(agent_msg) ((pull_error) *(agent_msg.event_data))

/* This states will be used by the update agent coroutines */
typedef enum agent_event_t {
    EVENT_INIT = 0,
    // EVENT CONTINUE
    EVENT_CONTINUE_START_,
    EVENT_SUBSCRIBE,
    EVENT_CHECKING_UPDATES,
    EVENT_CHECKING_UPDATES_TIMEOUT,
    EVENT_SEARCHING_SLOT,
    EVENT_CONN_RECEIVER,
    EVENT_RECEIVE,
    EVENT_VERIFY,
    EVENT_FINAL,
    EVENT_VERIFY_BEFORE,
    EVENT_VERIFY_AFTER,
    EVENT_APPLY,
    EVENT_CONTINUE_END_,
    // EVENT SEND
    EVENT_SEND_START_,
    EVENT_CHECKING_UPDATES_SEND,
    EVENT_RECEIVE_SEND,
    EVENT_SEND_END_,
    // EVENT RECOVER
    EVENT_RECOVER_START_,
    EVENT_CHECKING_UPDATES_RECOVER,
    EVENT_RECEIVE_RECOVER,
    EVENT_RECOVER_END_,
    // EVENT FAILURE
    EVENT_FAILURE_START_,
    EVENT_INIT_FAILURE,
    EVENT_SUBSCRIBE_FAILURE,
    EVENT_SEARCHING_SLOT_FAILURE,
    EVENT_SEARCHING_SLOT_FAILURE_2,
    EVENT_CONN_RECEIVER_FAILURE,
    EVENT_CONN_RECEIVER_FAILURE_2,
    EVENT_VERIFY_FAILURE,
    EVENT_INVALIDATE_OBJECT_FAILURE,
    EVENT_FAILURE_END_
} agent_event_t;

typedef struct agent_msg_t {
    agent_event_t event;
    void * event_data;
} agent_msg_t;

typedef struct {
    conn_config_t subscriber;
    conn_config_t receiver;
    uint8_t reuse_connection;
    identity_t identity;
    uint8_t* vendor_x;
    uint8_t* vendor_y;
    digest_func df;
    ecc_func_t ef;
    char* buffer;
    size_t buffer_size;
} update_agent_config;

static inline void update_agent_reuse_connection(update_agent_config* cfg, uint8_t reuse) {
    cfg->reuse_connection = reuse;
}

static inline void update_agent_set_identity(update_agent_config* cfg, identity_t identity) {
    cfg->identity = identity;
}

static inline void update_agent_vendor_keys(update_agent_config* cfg, uint8_t* x, uint8_t* y) {
    cfg->vendor_x = x;
    cfg->vendor_y = y;
}

static inline void update_agent_digest_func(update_agent_config* cfg, digest_func df) {
    cfg->df = df;
}

static inline void update_agent_ecc_func(update_agent_config* cfg, ecc_func_t ef) {
    cfg->ef = ef;
}

static inline void update_agent_set_buffer(update_agent_config* cfg, char* buffer, size_t buffer_size) {
    cfg->buffer = buffer;
    cfg->buffer_size = buffer_size;
}

typedef struct update_agent_ctx_t {
    txp_ctx stxp;
    subscriber_ctx sctx;
    receiver_ctx rctx;
    txp_ctx rtxp;
    mem_id_t id;
    mem_object_t new_obj;
    mem_object_t obj_t;
    pull_error err;
} update_agent_ctx_t;

agent_msg_t update_agent(update_agent_config* cfg, update_agent_ctx_t* ctx);

#endif /* AGENTS_UPDATE_H_ */
