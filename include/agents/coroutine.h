#ifndef AGENTS_COROUTINE_H_
#define AGENTS_COROUTINE_H_

#define PULL_BEGIN(state)  \
    static agent_t agent = { \
        .current_state = state, \
        .current_error = PULL_SUCCESS, \
        .required_action = CONTINUE, \
    }; \
    switch(agent.current_state) { case 0: \

#define PULL_CONTINUE(state) \
        do { \
            agent.current_state=state; \
            agent.current_error=PULL_SUCCESS; \
            agent.required_action=CONTINUE; \
            return agent; \
            case state:; \
        } while(0) \

#define PULL_SEND(state) \
        do { \
            agent.current_state=state; \
            agent.current_error=PULL_SUCCESS; \
            agent.required_action=SEND; \
            return agent; \
            case state:; \
        } while(0); \

#define PULL_RECOVER(state, error) \
        do { \
            agent.current_state=state; \
            agent.current_error=error; \
            agent.required_action=RECOVER; \
            return agent; \
            case state:; \
        } while (0); \

#define PULL_FAILURE(error) \
        do { \
            agent.current_error=error; \
            agent.required_action=FAILURE; \
            return agent; \
        } while (0); \

#define PULL_FINISH(state) } \
    agent.current_state=state; \
    agent.required_action=APPLY; \
    return agent; \

#endif /* AGENTS_COROUTINE_H_ */
