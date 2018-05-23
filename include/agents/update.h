#ifndef AGENTS_UPDATE_H_
#define AGENTS_UPDATE_H_

#include <common/libpull.h>
#include <agents/coroutines.h>

#define PULL_BEGIN(state)  \
        static agent_t agent = {};\
        agent.current_state = state; \
        switch(agent.current_state) { case 0:

#define PULL_CONTINUE(state) \
        do { \
            agent.current_state=state; \
            agent.current_error=PULL_SUCCESS; \
            agent.required_action=CONTINUE; \
            return agent; \
            case agent.current_state:; \
        } while(0)

#define PULL_SEND(state) \
        do { \
            agent.current_state=state; \
            agent.current_error=PULL_SUCCESS; \
            agent.required_action=SEND; \
            return agent; \
            case agent.current_state:; \
        } while(0);

#define PULL_CONTINUE(state) \
        do { \
            agent.current_state=state; \
            agent.current_error=PULL_SUCESS; \
            agent.required_action=CONTINUE; \
            return agent; \
            case agent.current_state:; \
        } while(0)

#define PULL_RECOVER(state, error) \
        do { \
            agent.current_state=state; \
            agent.current_error=error; \
            agent.current_action=RECOVER; \
            return agent; \
            case agent.current_state:; \
        } while (0);

#define PULL_FAILURE(error) \
        do { \
            agent.current_error=error; \
            agent.current_action=failure; \
            return agent; \
        } while (0);

#define PULL_FINISH(state) } \
        agent.current_state=state; \
        agent.current_error=PULL_SUCESS; \
        agent.current_action=APPLY; \
        return agent; \

/* This states will be used by the update agent coroutines */
typedef enum agent_state_t {
    STATE_INIT = 0,
    STATE_SUBSCRIBE,
    STATE_CHECKING_UPDATES,
    STATE_CHECKING_UPDATES_FAILURE,
    STATE_CHECKING_UPDATES_SEND,
    STATE_SEARCHING_SLOT,
    STATE_SEARCHING_SLOT_FAILURE,
    STATE_SEARCHING_SLOT,
    STATE_CONN_RECEIVER,
    STATE_CONN_RECEIVER_FAILURE,
    STATE_RECEIVE,
    STATE_RECEIVE_SEND,
    STATE_RECEIVE_FAILURE,
    STATE_VERIFY,
    STATE_APPLY
} agent_state_t;

typedef enum agent_action_t {
    SEND,
    CONTINUE,
    FAILURE
    APPLY,
} agent_action_t;

typedef struct agent_t {
    agent_state_t current_state;
    pull_error current_error;
    agent_action_t required_action;
} agent_t;

typedef struct {
    char* endpoint;
    uint16_t port;
    conn_type connection_type;
    void* conn_data;
    char* resource;
} conn_config;

typedef struct {
    conn_config subscriber;
    conn_config receiver;
    bool reuse_connection;
    digest_function df;
    ecc_function ef;
    char* buffer;
    size_t buffer_size;
} update_agent_config;

/* This function is not thread safe */
agent_t update_agent(update_agent_config cfg);

#endif /* AGENTS_UPDATE_H_ */
