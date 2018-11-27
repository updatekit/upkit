/** \file update.h
 * \brief Update agent provided by libpull.
 * \author Antonio Langiu
 * \defgroup ag_update
 * \brief
 * The update agent is the application using the libpull library to effectively
 * perform the update. It is in charge of communicating with the network and
 * coordinate the operations to successfully download, verify and apply the
 * update image. It should be normally executed in parallel to the standard
 * application. In this way, when an update is available, the device will
 * require the minimum time to obtain it.
 * \{
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#ifndef AGENTS_UPDATE_H_
#define AGENTS_UPDATE_H_

#include <libpull/common.h>
#include <libpull/network.h>
#include <libpull/memory.h>
#include <libpull/security.h>

#include <libpull_agents/coroutines.h>

#include "platform_headers.h"

#define IS_CONTINUE(agent_event) (agent_event > EVENT_CONTINUE_START_ && agent_event < EVENT_CONTINUE_END_ )
#define IS_SEND(agent_event) (agent_event > EVENT_SEND_START_ && agent_event < EVENT_SEND_END_ )
#define IS_RECOVER(agent_event) (agent_event > EVENT_RECOVER_START_ && agent_event < EVENT_RECOVER_END_ )
#define IS_FAILURE(agent_event) (agent_event > EVENT_FAILURE_START_ && agent_event < EVENT_FAILURE_END_ )

#define GET_CONNECTION(event_data) ((conn_ctx*) (event_data))
#define GET_ERROR(event_data) *((pull_error*) (event_data))

#define FOREACH_IGNORED_EVENT(ACTION) \
    ACTION(EVENT_CONTINUE_START_) \
    ACTION(EVENT_CONTINUE_END_) \
    ACTION(EVENT_SEND_START_) \
    ACTION(EVENT_SEND_END_) \
    ACTION(EVENT_RECOVER_START_) \
    ACTION(EVENT_RECOVER_END_) \
    ACTION(EVENT_FAILURE_START_) \
    ACTION(EVENT_FAILURE_END_)


/** \brief This states will be used by the update agent coroutines */
typedef enum agent_event_t {
    EVENT_INIT = 0,
    // EVENT CONTINUE
    EVENT_CONTINUE_START_,
    EVENT_SUBSCRIBE,
    EVENT_GET_MSG,
    EVENT_RECEIVE,
    EVENT_VERIFY,
    EVENT_FINAL,
    EVENT_APPLY,
    EVENT_VERIFY_BEFORE,
    EVENT_SUBSCRIBE_TIMEOUT,
    EVENT_VERIFY_AFTER,
    EVENT_CONTINUE_END_,
    // EVENT SEND
    EVENT_SEND_START_,
    EVENT_SUBSCRIBE_SEND,
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
    EVENT_INIT_FAILURE_2,
    EVENT_INIT_FAILURE_3,
    EVENT_GET_MSG_FAILURE,
    EVENT_SUBSCRIBE_FAILURE,
    EVENT_SUBSCRIBE_FAILURE_2,
    EVENT_RECEIVER_FAILURE,
    EVENT_RECEIVER_FAILURE_2,
    EVENT_VERIFY_FAILURE,
    EVENT_INVALIDATE_OBJECT_FAILURE,
    EVENT_FAILURE_END_
} agent_event_t;

/**
 * \brief  Configuration structure for the update agent.
 */
typedef struct update_agent_config_t {
    conn_config_t* conn;
    safestore_t* safestore;
} update_agent_config_t;

static inline void update_agent_config(update_agent_config_t* cfg, conn_config_t* conn, safestore_t* safestore) {
    cfg->conn = conn;
    cfg->safestore = safestore;
}

/**
 * \brief Context of the update agent.
 */
typedef struct update_agent_ctx_t {
    mem_id_t id;
    mem_object_t new_obj;
    mem_object_t obj_t;
    pull_error err;
    fsm_ctx_t fsmc;
    conn_ctx conn;
    request_ctx_t reqc;
    receiver_msg_t msg;
} update_agent_ctx_t;

/**
 * \brief  Function to execute the update agent. This function will return
 * several times, each time with a different message indicating the state of the
 * update agent. In this way you can interact with the update agent modifying
 * the states.
 *
 * \param cfg Pointer to the configuration structure.
 * \param ctx Pointer to the update agent context.
 *
 * \returns Messages containing the current event.
 */
agent_event_t update_agent(update_agent_config_t* cfg, update_agent_ctx_t* ctx, agent_data_t* agent_data);

#endif /* AGENTS_UPDATE_H_ */
