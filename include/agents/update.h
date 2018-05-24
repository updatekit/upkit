#ifndef AGENTS_UPDATE_H_
#define AGENTS_UPDATE_H_

#include <common/libpull.h>
#include <agents/coroutine.h>
#include <network/transport.h>
#include <security/digest.h>
#include <security/ecc.h>
#include <network/receiver.h>
#include <network/subscriber.h>
#include <network/transport_config.h>

#include "platform_headers.h"

/* This states will be used by the update agent coroutines */
typedef enum agent_state_t {
    STATE_INIT = 0,
    STATE_SUBSCRIBE,
    STATE_CHECKING_UPDATES,
    STATE_CHECKING_UPDATES_FAILURE,
    //STATE_CHECKING_UPDATES_SEND,
    STATE_SEARCHING_SLOT,
    //STATE_SEARCHING_SLOT_FAILURE,
    STATE_CONN_RECEIVER,
    STATE_CONN_RECEIVER_FAILURE,
    STATE_RECEIVE,
    //STATE_RECEIVE_SEND,
    STATE_RECEIVE_FAILURE,
    STATE_VERIFY,
    STATE_FINAL,
    STATE_APPLY
} agent_state_t;

typedef enum agent_action_t {
    SEND,
    CONTINUE,
    FAILURE,
    RECOVER,
    APPLY,
} agent_action_t;

typedef struct agent_t {
    agent_state_t current_state;
    pull_error current_error;
    agent_action_t required_action;
} agent_t;

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


inline void update_agent_reuse_connection(update_agent_config* cfg, uint8_t reuse) {
    cfg->reuse_connection = reuse;
}

inline void update_agent_vendor_keys(update_agent_config* cfg, uint8_t* x, uint8_t* y) {
    cfg->vendor_x = x;
    cfg->vendor_y = y;
}

inline void update_agent_digest_func(update_agent_config* cfg, digest_func df) {
    cfg->df = df;
}

inline void update_agent_ecc_func(update_agent_config* cfg, ecc_func_t ef) {
    cfg->ef = ef;
}

inline void update_agent_set_buffer(update_agent_config* cfg, char* buffer, size_t buffer_size) {
    cfg->buffer = buffer;
    cfg->buffer_size = buffer_size;
}

typedef struct update_agent_ctx_t {
    txp_ctx stxp;
    subscriber_ctx sctx;
    receiver_ctx rctx;
    txp_ctx rtxp;
    obj_id id;
    mem_object new_obj;
    mem_object obj_t;
    pull_error err;
} update_agent_ctx_t;

agent_t update_agent(update_agent_config* cfg, update_agent_ctx_t* ctx);

#endif /* AGENTS_UPDATE_H_ */
