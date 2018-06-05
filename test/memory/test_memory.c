#include <libpull/common.h>
#include <libpull/memory.h>

#include "support/support.h"

#include <string.h>

mem_object_t object;

#define FOREACH_TEST(DO) \
    DO(memory_read_sequential, 0) \
    DO(memory_read_random, 0) \
    DO(memory_write_sequential, 0) \
    DO(memory_write_random, 0) \
    DO(memory_invalid_object, 0)

TEST_RUNNER();

const char content[12] = {'m', 'e', 'm', 'o', 'r', 'y', '_', 't', 'e', 's', 't', '\n'};
#define SIZE sizeof(content)
#define REP 64

void setUp(void) {
    TEST_ASSERT(memory_open(&object, TEST_MEMORY_FILE, WRITE_ALL) == PULL_SUCCESS);
}

void tearDown(void) {
    TEST_ASSERT(memory_close(&object) == PULL_SUCCESS);
}

void test_memory_read_sequential(void) {
    TEST_ASSERT_MESSAGE(object.fp > 0, "Invalid File pointer");
    int i = 0;
    unsigned char buffer[SIZE*REP];
    TEST_ASSERT_EQUAL_INT(SIZE*REP, memory_read(&object, (unsigned char*) buffer, SIZE*REP, 0x0));
    int valid = 1;
    for (i=0; i<SIZE*REP; i+=SIZE) {
        if(strncmp((const char*) &buffer[i], content, sizeof(content)) != 0) {
            valid = 0;
        }
    }
    TEST_ASSERT_MESSAGE(valid, "The readed content is not the one expected");
}

void test_memory_read_random(void) {
    TEST_ASSERT_MESSAGE(object.fp > 0, "Invadlid File pointer");
    uint8_t buffer[SIZE];
    TEST_ASSERT_EQUAL_INT(memory_read(&object, buffer, SIZE, SIZE*30), SIZE);
    TEST_ASSERT_MESSAGE(strncmp((char*) buffer, content, sizeof(content)) == 0,
            "The readed content is not the one expected");
}

void test_memory_write_sequential(void) {
    TEST_ASSERT_MESSAGE(object.fp > 0, "Invadlid File pointer");
    int i = 0, size = 0;
    for (i=0; i<SIZE*REP; i+=SIZE) {
        size += memory_write(&object, (uint8_t*) content, SIZE, i);
    }
    TEST_ASSERT_EQUAL_INT(size, SIZE*REP);
    test_memory_read_sequential();
}

void test_memory_write_random(void) {
    TEST_ASSERT_MESSAGE(object.fp > 0, "Invadlid File pointer");
    TEST_ASSERT_EQUAL_INT(memory_write(&object, (const uint8_t*) content, SIZE, 0x0), SIZE);
    test_memory_read_sequential();
}

void test_memory_invalid_object(void) {
    mem_object_t invalid_object;
    pull_error err = memory_open(&invalid_object, -1, WRITE_ALL);
    TEST_ASSERT_EQUAL(MEMORY_MAPPING_ERROR, err);
    err = memory_open(&invalid_object, 120, WRITE_ALL);
    TEST_ASSERT_EQUAL(MEMORY_MAPPING_ERROR, err);
}
