#ifndef AGENTS_BOOTLOADER_H_
#define AGENTS_BOOTLOADER_H_

#include <libpull/common.h>
#include <libpull/network.h>
#include <libpull/memory.h>
#include <libpull/security.h>

#include <libpull_agents/coroutines.h>
#include <libpull_agents/bootloader_ctx.h>

#include "platform_headers.h"

#define IS_CONTINUE(agent_msg) (agent_msg.event > EVENT_CONTINUE_START_ && agent_msg.event < EVENT_CONTINUE_STOP_ )
#define IS_FAILURE(agent_msg) (agent_msg.event > EVENT_FAILURE_START_ && agent_msg.event < EVENT_FAILURE_STOP_ )

#define GET_BOOT_ID(agent_msg) *((mem_id_t*) (agent_msg.event_data))
#define GET_ERROR(agent_msg) *((pull_error*) (agent_msg.event_data))

 #define FOREACH_IGNORED_EVENT(ACTION) \
     ACTION(EVENT_CONTINUE_START_) \
     ACTION(EVENT_CONTINUE_STOP_) \
     ACTION(EVENT_FAILURE_START_) \
     ACTION(EVENT_FAILURE_STOP_)

typedef enum agent_event_t {
    EVENT_INIT = 0,
    // EVENT CONTINUE
    EVENT_CONTINUE_START_,
    EVENT_BOOT,
    EVENT_VALIDATE_BOOTABLE_START,
    EVENT_VALIDATE_BOOTABLE_STOP,
    EVENT_BOOTSTRAP,
    EVENT_FIRST_BOOT,
    EVENT_GET_NEWEST_FIRMWARE,
    EVENT_GET_NEWEST_NON_BOOTABLE,
#if RECOVERY_IMAGE
    EVENT_STORE_RECOVERY,
    EVENT_STORE_RECOVERY_COPY_START,
    EVENT_STORE_RECOVERY_COPY_STOP,
    EVENT_RECOVERY_RESTORE,
    EVENT_RECOVERY_RESTORE_START,
    EVENT_RECOVERY_RESTORE_STOP,
#endif
    EVENT_STORE_BOOTLAODER_CTX,
    EVENT_UPGRADE,
    EVENT_UPGRADE_COPY_START,
    EVENT_UPGRADE_COPY_STOP,
    EVENT_UPGRADE_SUCCESS,
    EVENT_VALIDATE_NON_BOOTABLE,
    EVENT_VALIDATE_NON_BOOTABLE_START,
    EVENT_VALIDATE_NON_BOOTABLE_STOP,
    EVENT_CONTINUE_STOP_,
    // EVENT_FAILURE
    EVENT_FAILURE_START_,
    EVENT_VALIDATE_BOOTABLE_FAILURE,
    EVENT_BOOTSTRAP_FAILURE,
    EVENT_BOOTSTRAP_FAILURE_2,
    EVENT_FATAL_FAILURE,
    EVENT_FIRST_BOOT_FAILURE,
    EVENT_GET_NEWEST_FIRMWARE_FAILURE,
    EVENT_GET_NEWEST_FIRMWARE_FAILURE_2,
    EVENT_GET_NEWEST_NON_BOOTABLE_FAILURE,
    EVENT_STORE_BOOTLAODER_CTX_FAILURE,
#if RECOVERY_IMAGE
    EVENT_STORE_RECOVERY_COPY_FAILURE,
    EVENT_STORE_RECOVER_FAILURE,
#endif
    EVENT_UPGRADE_COPY_FAILURE,
    EVENT_UPGRADE_FAILURE,
    EVENT_UPGRADE_FAILURE_2,
    EVENT_VALIDATE_NON_BOOTABLE_INVALID,
    EVENT_VALIDATE_NON_BOOTABLE_FAILURE,
    EVENT_FAILURE_STOP_,
    // Other events
    EVENT_FINISH
} agent_event_t;

typedef struct agent_msg_t {
    agent_event_t event;
    void * event_data;
} agent_msg_t;

typedef struct bootloader_agent_config {
    mem_id_t bootloader_ctx_id;
    mem_id_t recovery_id;
    uint8_t* vendor_x;
    uint8_t* vendor_y;
    digest_func df;
    ecc_func_t ef;
    uint8_t* buffer;
    size_t buffer_size;
} bootloader_agent_config;


static inline void bootloader_agent_vendor_keys(bootloader_agent_config* cfg, uint8_t* x, uint8_t* y) {
    cfg->vendor_x = x;
    cfg->vendor_y = y;
}

static inline void bootloader_agent_digest_func(bootloader_agent_config* cfg, digest_func df) {
    cfg->df = df;
}

static inline void bootloader_agent_ecc_func(bootloader_agent_config* cfg, ecc_func_t ef) {
    cfg->ef = ef;
}

static inline void bootloader_agent_set_buffer(bootloader_agent_config* cfg, uint8_t* buffer, size_t buffer_size) {
    cfg->buffer = buffer;
    cfg->buffer_size = buffer_size;
}

agent_msg_t bootloader_agent(bootloader_agent_config* cfg);

#endif /* AGENTS_BOOTLOADER_H_ */
