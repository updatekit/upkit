#ifndef AGENTS_COROUTINE_H_
#define AGENTS_COROUTINE_H_

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

#endif /* AGENTS_COROUTINE_H_ */
