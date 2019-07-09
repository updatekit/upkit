#ifndef MEMORY_FILE_POSIX_H_
#define MEMORY_FILE_POSIX_H_

#ifdef WITH_PLATFORM_POSIX

#define SWAP_SIZE 0x1000
#include <libpull/common.h>

struct mem_object_t {
    char* path;
    int fp;
    int start_offset;
    int end_offset;
};

enum memory_objects_enum {
    OBJ_FIRST = 0,
    BOOTLOADER_CTX, //1
    OBJ_GOLD, //2
    OBJ_A, //3 
    OBJ_B, //4
    OBJ_C,
    OBJ_SWAP,
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
