#ifndef MEMORY_FILE_POSIX_H_
#define MEMORY_FILE_POSIX_H_

#ifdef WITH_PLATFORM_POSIX

#include <libpull/common.h>

struct mem_object_t {
    char* path;
    int fp;
    int start_offset;
    int end_offset;
    manifest_t mt;
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
void copy_asset(char* src, char* dest);
void restore_assets(void);
/***** End Testing functions *****/

#endif /* WITH_PLATFORM_POSIX */
#endif /* MEMORY_FILE_POSIX_H_ */
