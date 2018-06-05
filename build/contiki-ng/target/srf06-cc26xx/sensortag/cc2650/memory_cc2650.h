#ifndef MEMORY_CC2650_H_
#define MEMORY_CC2650_H_

#include <libpull/memory.h>

enum memory_type {
    EXTERNAL,
    INTERNAL,
    INVALID
};

enum memory_objects_enum {
    BOOTLOADER,     // 0
    BOOTLOADER_CTX, // 1
    OBJ_GOLD,       // 2
    OBJ_RUN,        // 3
    OBJ_1,          // 4
    OBJ_2           // 5
};

#endif // MEMORY_CC2650_H_
