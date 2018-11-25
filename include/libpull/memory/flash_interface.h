/** \file flash_interface.h
 * \brief Interface used to easily implement the memory interface when using
 * flash.
 * \author Antonio Langiu
 * \defgroup mem_flashint
 * \{
 */
#ifndef LIBPULL_MEMORY_FLASH_INTERFACE_H_
#define LIBPULL_MEMORY_FLASH_INTERFACE_H_

#include <libpull/common.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if MEMORY_FLASH_IMPL == 1

// All these functions should return < 0 on error and >= 0 in case of success
typedef int (*const _flash_open) (void);
typedef int (*const _flash_erase) (address_t offset, size_t page_size);
typedef int (*const _flash_program) (const uint8_t* buffer, address_t offset, size_t size);
typedef int (*const _flash_read) (uint8_t* buffer, address_t offset, size_t size);
typedef int (*const _flash_close) (void);
typedef void (*const _watchdog_rst) (void);

uint32_t get_start_offset(mem_id_t id);
uint32_t get_end_offset(mem_id_t id);

typedef struct flash_descr_t {
    size_t page_size;
    _flash_open open;
    _flash_erase erase;
    _flash_program program;
    _flash_read read;
    _flash_close close;
    _watchdog_rst rst;
} flash_descr_t;

typedef struct mem_object_t {
    uint32_t start;
    uint32_t end;
    flash_descr_t* fdescr;
    mem_mode_t mode;
} mem_object_t;

extern mem_object_t flash_objects[];

#ifndef WITH_CUSTOM_OBJECTS

enum memory_objects_enum {
    BOOTLOADER = 0, // 0
    BOOTLOADER_CTX, // 1
    OBJ_1,          // 2
    OBJ_2,          // 3
    SWAP            // 4
};

#endif /* WITH_CUSTOM_OBJECTS */

#endif /* MEMORY_FLASH_IMPL == 1 */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_MEMORY_FLASH_INTERFACE_H_ */
