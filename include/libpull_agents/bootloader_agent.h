/** \file bootloader_agent.h
 * \brief Bootloader agent already provided by libpull.
 * \author Antonio Langiu
 * \defgroup ag_bl
 * \{
 */
#ifndef AGENTS_BOOTLOADER_H_
#define AGENTS_BOOTLOADER_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <libpull/common.h>
#include <libpull/network.h>
#include <libpull/memory.h>
#include <libpull/security.h>

#include <libpull_agents/coroutines.h>
#include <libpull_agents/bootloader_ctx.h>

#include "platform_headers.h"

#define IS_CONTINUE(event) (event > EVENT_CONTINUE_START_ && event < EVENT_CONTINUE_STOP_ )
#define IS_FAILURE(event) (event > EVENT_FAILURE_START_ && event < EVENT_FAILURE_STOP_ )

#define GET_BOOT_ID(event_data) *((mem_id_t*) (event_data))
#define GET_ERROR(event_data) *((pull_error*) (event_data))

 #define FOREACH_IGNORED_EVENT(ACTION) \
     ACTION(EVENT_CONTINUE_START_) \
     ACTION(EVENT_CONTINUE_STOP_) \
     ACTION(EVENT_FAILURE_START_) \
     ACTION(EVENT_FAILURE_STOP_)

/** 
 * \brief  Events returned by the bootloader agent.
 */
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
#if FACTORY_IMAGE
    EVENT_STORE_FACTORY,
    EVENT_STORE_FACTORY_COPY_START,
    EVENT_STORE_FACTORY_COPY_STOP,
    EVENT_FACTORY_RESET,
    EVENT_FACTORY_RESET_START,
    EVENT_FACTORY_RESET_STOP,
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
#if FACTORY_IMAGE
    EVENT_STORE_FACTORY_COPY_FAILURE,
    EVENT_STORE_RECOVER_FAILURE,
#endif
    EVENT_UPGRADE_COPY_FAILURE,
    EVENT_UPGRADE_FAILURE,
    EVENT_UPGRADE_FAILURE_2,
    EVENT_UPGRADE_FAILURE_3,
    EVENT_UPGRADE_FAILURE_4,
    EVENT_VALIDATE_NON_BOOTABLE_INVALID,
    EVENT_VALIDATE_NON_BOOTABLE_FAILURE,
    EVENT_FAILURE_STOP_,
    // Other events
    EVENT_FINISH
} agent_event_t;

/** 
 * \brief  Configuration structure of the bootloader agent.
 */
typedef struct bootloader_agent_config {
    mem_id_t bootloader_ctx_id;
    mem_id_t recovery_id;
    mem_id_t swap_id;
    size_t swap_size;
    uint8_t* vendor_x;
    uint8_t* vendor_y;
    digest_func df;
    ecc_func_t ef;
    uint8_t* buffer;
    size_t buffer_size;
} bootloader_agent_config;


/** 
 * \brief  Function to configure the vendor keys.
 * 
 * \param cfg Pointer to configuration structure.
 * \param x X component of the vendor key.
 * \param y Y component of the vendor key.
 */
static inline void bootloader_agent_vendor_keys(bootloader_agent_config* cfg, uint8_t* x, uint8_t* y) {
    cfg->vendor_x = x;
    cfg->vendor_y = y;
}

/** 
 * \brief  Function to configure the bootloader digest function.
 * 
 * \param cfg Pointer to the configuration structure.
 * \param df Digest function to be used. (For a list of them check the
 * security/digest documentation).
 */
static inline void bootloader_agent_digest_func(bootloader_agent_config* cfg, digest_func df) {
    cfg->df = df;
}

/** 
 * \brief  Function to configure the bootloader ECC function.
 * 
 * \param cfg Pointer to the configuration structure.
 * \param ef ECC function to be used. (For a list of them check the
 * security/ecc documentation).
 */
static inline void bootloader_agent_ecc_func(bootloader_agent_config* cfg, ecc_func_t ef) {
    cfg->ef = ef;
}

/** 
 * \brief  Function to set the buffer used by the bootloader.
 * \note When using flash memory, to optimize the IO performance the buffer size should be equal to the
 * page size of your flash memory.
 * \param cfg Pointer to the configuration structure.
 * \param buffer Pointer to the buffer.
 * \param buffer_size Size of the buffer.
 */
static inline void bootloader_agent_set_buffer(bootloader_agent_config* cfg, uint8_t* buffer, size_t buffer_size) {
    cfg->buffer = buffer;
    cfg->buffer_size = buffer_size;
}

/** 
 * \brief  Function executing the bootloader agent.
 * This function is internally build with a set of coroutines that stops the
 * function at different execution points. This allows you to perform several
 * actions during according to the returned phase, to check for errors and try
 * to recover, etc.
 * \param cfg Pointer to the configuration structure.
 * 
 * \returns   Structure indicating the current state and the data related to it.
 */
agent_event_t bootloader_agent(bootloader_agent_config* cfg, void** event_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} AGENTS_BOOTLOADER_H_ */
