#ifndef TEST_PLATFORM_MEMORY_H_
#define TEST_PLATFORM_MEMORY_H_

#include <libpull/memory.h>
#include "ntest.h"

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
void ntest_prepare(void) {
    for (int i=0; i<MEMORY_TEST_BUFLEN; i++) {
        memory_buffer[i] = i;
        memory_test[i] = 0;
    }
}

/* This function will be executed after each test */
void ntest_clean(void) {}

void test_memory(void) {
    mem_object_t obj;
    pull_error err = memory_open(&obj, TESTING_MEMORY_OBJECT, WRITE_ALL);
    nTEST_COMPARE_INT(PULL_SUCCESS, err, err_as_str(err));
    int value = memory_read(&obj, memory_test, sizeof(memory_test), 0);
    nTEST_COMPARE_INT(sizeof(memory_test), value, "Error reading ereased memory");
    value = memory_write(&obj, memory_buffer, sizeof(memory_buffer), 0);
    nTEST_COMPARE_INT(sizeof(memory_buffer), value, "Error writing buffer");
    err = memory_close(&obj);
    nTEST_COMPARE_INT(PULL_SUCCESS, err, err_as_str(err));
    err = memory_open(&obj, TESTING_MEMORY_OBJECT, READ_ONLY);
    nTEST_COMPARE_INT(PULL_SUCCESS, err, err_as_str(err));
    value = memory_read(&obj, memory_test, sizeof(memory_test), 0);
    nTEST_COMPARE_INT(sizeof(memory_test), value, "Error reading memory");
    nTEST_COMPARE_MEM(memory_buffer, memory_test, sizeof(memory_test),
            "The readed data is different from the writted one\n");
    err = memory_close(&obj);
    nTEST_COMPARE_INT(PULL_SUCCESS, err, err_as_str(err));
}

#endif /* TEST_PLATFORM_MEMORY_H_ */
