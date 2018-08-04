#ifndef AGENTS_COROUTINE_H_
#define AGENTS_COROUTINE_H_

#define PULL_BEGIN(ev)  \
    static agent_event_t current_event = ev; \
    static agent_msg_t agent_msg; \
    switch(current_event) { \
        FOREACH_IGNORED_EVENT(IGNORE_EVENT) \
        case ev:

#define PULL_CONTINUE(ev, ev_data) \
    do { \
        current_event = ev; \
        agent_msg.event = ev; \
        agent_msg.event_data = ev_data; \
        return agent_msg; \
        case ev:; \
    } while(0)

#define PULL_FINISH(ev) \
        case ev: \
            agent_msg.event = ev; \
            agent_msg.event_data = NULL; \
            return agent_msg; \
    } \
    return agent_msg;



#define IGNORE_EVENT(event) case event:

#endif /* AGENTS_COROUTINE_H_ */
