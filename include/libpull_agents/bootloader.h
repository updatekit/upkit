#ifndef AGENTS_BOOTLOADER_H_
#define AGENTS_BOOTLOADER_H_

#include <libpull/common.h>
#include <libpull/network.h>
#include <libpull/memory.h>
#include <libpull/security.h>

#include <libpull_agents/coroutines.h>

#include "platform_headers.h"

#define IS_CONTINUE(agent_msg) (agent_msg.event > EVENT_CONTINUE_START_ && agent_msg.event < EVENT_CONTINUE_END_ )
#define IS_RECOVER(agent_msg) (agent_msg.event > EVENT_RECOVER_START_ && agent_msg.event < EVENT_SEND_END_ )
#define IS_FAILURE(agent_msg) (agent_msg.event > EVENT_FAILURE_START_ && agent_msg.event < EVENT_FAILURE_END_ )

#define GET_ERROR(agent_msg) ((pull_error) *(agent_msg.event_data))

typedef enum agent_event_t {
    EVENT_INIT = 0,




} agent_event_t;

typedef struct agent_msg_t {
    agent_event_t event;
    void * event_data;
} agent_msg_t;

typedef struct bootloader_agent_config {
    mem_id_t bootloader_ctx_id;
    mem_id_t recovery_id;
} bootloader_agent_config;

agent_msg_t bootloader_agent(bootloader_agent_config* cfg);

#endif /* AGENTS_BOOTLOADER_H_ */
