#ifndef LIBPULL_MEMORY_FLASH_INTERFACE_H_
#define LIBPULL_MEMORY_FLASH_INTERFACE_H_

#include <libpull/common.h>

// All these functions should return < 0 on error and >= 0 in case of success
typedef int (*flash_open) (void);
typedef int (*flash_erase) (address_t offset, size_t page_size);
typedef int (*flash_program) (const uint8_t* buffer, address_t offset, size_t size);
typedef int (*flash_read) (uint8_t* buffer, address_t offset, size_t size);
typedef int (*flash_close) (void);

uint32_t get_start_offset(mem_id_t id);
uint32_t get_end_offset(mem_id_t id);

typedef struct flash_descr_t {
    size_t page_size;
    flash_open open;
    flash_erase erase;
    flash_program program;
    flash_read read;
    flash_close close;
} flash_descr_t;

typedef struct mem_object_t {
    uint32_t start;
    uint32_t end;
    flash_descr_t* fdescr;
    mem_mode_t mode;
} mem_object_t;

extern mem_object_t flash_objects[];

#endif /* LIBPULL_MEMORY_FLASH_INTERFACE_H_ */