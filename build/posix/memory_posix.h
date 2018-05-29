#ifndef MEMORY_FILE_POSIX_H_
#define MEMORY_FILE_POSIX_H_

#include <libpull/common.h>
#include <libpull/memory.h>

struct mem_object_t {
    char* path;
    int fp;
    int start_offset;
    int end_offset;
};

enum memory_objects_enum {
    OBJ_FIRST = 0,
    BOOTLOADER_CTX,
    OBJ_GOLD,
    OBJ_A,
    OBJ_B,
    OBJ_C,
    /// Other files used for testing
    TEST_MEMORY_FILE,
    OBJ_LAST
};

/******* Testing functions *******/
void override_memory_object(mem_id_t id, char* path);
/***** End Testing functions *****/

#endif /* MEMORY_FILE_POSIX_H_ */
