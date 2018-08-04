#include <libpull/common.h>
#include <libpull/memory.h>

#include "support/support.h"

#include <string.h>

mem_object_t object;
const char content[12] = {'m', 'e', 'm', 'o', 'r', 'y', '_', 't', 'e', 's', 't', '\n'};
#define SIZE sizeof(content)
#define REP 64

void ntest_prepare(void) {
    nTEST_TRUE(memory_open(&object, TEST_MEMORY_FILE, WRITE_ALL) == PULL_SUCCESS);
}

void ntest_clean(void) {
    nTEST_TRUE(memory_close(&object) == PULL_SUCCESS);
}

void test_memory_read_sequential(void) {
    nTEST_TRUE(object.fp > 0, "Invalid File pointer");
    int i = 0;
    unsigned char buffer[SIZE*REP];
    nTEST_COMPARE_INT(SIZE*REP, memory_read(&object, (unsigned char*) buffer, SIZE*REP, 0x0));
    int valid = 1;
    for (i=0; i<SIZE*REP; i+=SIZE) {
        if(strncmp((const char*) &buffer[i], content, sizeof(content)) != 0) {
            valid = 0;
        }
    }
    nTEST_TRUE(valid, "The readed content is not the one expected");
}

void test_memory_read_random(void) {
    nTEST_TRUE(object.fp > 0, "Invadlid File pointer");
    uint8_t buffer[SIZE];
    nTEST_COMPARE_INT(memory_read(&object, buffer, SIZE, SIZE*30), SIZE);
    nTEST_TRUE(strncmp((char*) buffer, content, sizeof(content)) == 0,
            "The readed content is not the one expected");
}

void test_memory_write_sequential(void) {
    nTEST_TRUE(object.fp > 0, "Invadlid File pointer");
    int i = 0, size = 0;
    for (i=0; i<SIZE*REP; i+=SIZE) {
        size += memory_write(&object, (uint8_t*) content, SIZE, i);
    }
    nTEST_COMPARE_INT(size, SIZE*REP);
    test_memory_read_sequential();
}

void test_memory_write_random(void) {
    nTEST_TRUE(object.fp > 0, "Invadlid File pointer");
    nTEST_COMPARE_INT(memory_write(&object, (const uint8_t*) content, SIZE, 0x0), SIZE);
    test_memory_read_sequential();
}

void test_memory_invalid_object(void) {
    mem_object_t invalid_object;
    pull_error err = memory_open(&invalid_object, -1, WRITE_ALL);
    nTEST_COMPARE_INT(MEMORY_MAPPING_ERROR, err);
    err = memory_open(&invalid_object, 120, WRITE_ALL);
    nTEST_COMPARE_INT(MEMORY_MAPPING_ERROR, err);
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_memory_read_sequential);
    nTEST_RUN(test_memory_read_random);
    nTEST_RUN(test_memory_write_sequential);
    nTEST_RUN(test_memory_write_random);
    nTEST_RUN(test_memory_invalid_object);
    nTEST_END();
}
