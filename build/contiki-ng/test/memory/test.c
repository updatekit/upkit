#include "contiki.h"
#include "memory_firefly.h"
#include <stdio.h>
#include <string.h>

#define CHECK(cond, ...) \
    if (cond) { \
        printf(__VA_ARGS__); \
        goto final; \
    } else { \
        printf("pass: "__VA_ARGS__); \
    }

#define BUFFER_SIZE 200

PROCESS(memory_test, "Test memory");
AUTOSTART_PROCESSES(&memory_test);

PROCESS_THREAD(memory_test, ev, data) {
    PROCESS_BEGIN();
    int i;
    int memory_buffer[BUFFER_SIZE];
    int memory_test[BUFFER_SIZE];
    for (i=0; i<BUFFER_SIZE; i++) {
        memory_buffer[i] = i;
        memory_test[i] = 0;
    }
    mem_object_t obj;
    pull_error err = memory_open(&obj, OBJ_2, WRITE_ALL);
    CHECK(err, "Error opening memory object 2 %s\n", err_as_str(err));
    int value = memory_read(&obj, memory_test, sizeof(memory_test), 0);
    CHECK(value != sizeof(memory_test), "Error reading ereased memory\n");

    value = memory_write(&obj, memory_buffer, sizeof(memory_buffer), 0);
    CHECK(value != sizeof(memory_buffer), "Error writing buffer");
    err = memory_close(&obj);
    CHECK(err, "Error closing object %s\n", err_as_str(err));
    err = memory_open(&obj, OBJ_2, READ_ONLY);
    CHECK(err, "Error reopening in reading mode %s\n", err_as_str(err));
    value = memory_read(&obj, memory_test, sizeof(memory_test), 0);
    CHECK(value != sizeof(memory_test), "Error reading memory %s\n", err_as_str(err));
    int res = memcmp(memory_buffer, memory_test, sizeof(memory_test));
    CHECK(res != 0, "The readed data is different from the writted one\n");
    printf("Success\n");
final:
    for (i=0; i<BUFFER_SIZE; i++) {
        if (memory_buffer[i] != memory_test[i]) {
            printf("%0x != %0x, ", memory_buffer[i], memory_test[i]);
        }
    }
    while (1) {
       // NOP 
    }
    PROCESS_END();
}
