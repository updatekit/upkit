#ifndef MEMORY_NRF52840_PCA10056_H_
#define MEMORY_NRF52840_PCA10056_H_

#include <libpull/memory.h>

enum memory_objects_enum {
    BOOTLOADER = 0, // 0
    BOOTLOADER_CTX, // 1
    OBJ_1,          // 2
    OBJ_2,          // 3
    SWAP            // 4
};

#endif /* MEMORY_NRF52840_PCA10056_H_ */
