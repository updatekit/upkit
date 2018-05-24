#ifndef MEMORY_FILE_POSIX_H_
#define MEMORY_FILE_POSIX_H_

#include <common/libpull.h>
#include <memory/memory.h>

struct mem_object_t {
    char* path;
    int fp;
    int start_offset;
    int end_offset;
};

enum memory_objects_enum {
    OBJ_FIRST = 0,
    OBJ_GOLD = 1,
    OBJ_RUN = 2,
    OBJ_1 = 3,
    OBJ_2 = 4,
    /// Other files used for testing
    TEST_MEMORY_FILE = 5,
    OBJ_LAST = 6
};

/******* Testing functions *******/
void override_memory_object(mem_id_t id, char* path, int start, int end);
/***** End Testing functions *****/

#endif // MEMORY_FILE_POSIX_H_
