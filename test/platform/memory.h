#ifndef TEST_PLATFORM_MEMORY_H_
#define TEST_PLATFORM_MEMORY_H_

#include <libpull/memory.h>
#include "shared.h"

#include "platform_headers.h"

#ifndef MEMORY_TEST_BUFLEN
#define MEMORY_TEST_BUFLEN 200
#endif /* MEMORY_TEST_BUFLEN */

#ifndef TESTING_MEMORY_OBJECT
#define TESTING_MEMORY_OBJECT OBJ_2
#endif /* TESTING_MEMORY_OBJECT */

static int memory_buffer[MEMORY_TEST_BUFLEN];
static int memory_test[MEMORY_TEST_BUFLEN];

/* This function will be executed before each test */
void setUp(void) {
    for (int i=0; i<MEMORY_TEST_BUFLEN; i++) {
        memory_buffer[i] = i;
        memory_test[i] = 0;
    }
}

/* This function will be executed after each test */
void tearDown(void) {}

DEFINE_TEST(test_memory) {
    mem_object_t obj;
    pull_error err = memory_open(&obj, TESTING_MEMORY_OBJECT, WRITE_ALL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(PULL_SUCCESS, err, err_as_str(err));
    int value = memory_read(&obj, memory_test, sizeof(memory_test), 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(sizeof(memory_test), value, "Error reading ereased memory");
    value = memory_write(&obj, memory_buffer, sizeof(memory_buffer), 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(sizeof(memory_buffer), value, "Error writing buffer");
    err = memory_close(&obj);
    TEST_ASSERT_EQUAL_INT_MESSAGE(PULL_SUCCESS, err, err_as_str(err));
    err = memory_open(&obj, TESTING_MEMORY_OBJECT, READ_ONLY);
    TEST_ASSERT_EQUAL_INT_MESSAGE(PULL_SUCCESS, err, err_as_str(err));
    value = memory_read(&obj, memory_test, sizeof(memory_test), 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(sizeof(memory_test), value, "Error reading memory");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(memory_buffer, memory_test, sizeof(memory_test),
            "The readed data is different from the writted one\n");
    err = memory_close(&obj);
    TEST_ASSERT_EQUAL_INT_MESSAGE(PULL_SUCCESS, err, err_as_str(err));
}

#endif /* TEST_PLATFORM_MEMORY_H_ */
