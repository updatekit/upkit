#include "unity.h"
#include "sample_data.h"
#include "digest.h"
#include "error.h"
#include "ecc.h"
#include "memory_objects.h"
#include "verifier.h"
#include "manifest.h"
#include "memory_file_posix.h"
#include "simple_manifest_impl.h"
#include "invalid_digest.h"

#include "mock_memory.h"
#define NO_MOCK(func) func##_StubWithCallback(func##_impl)

#define BUFFER_LEN 1024
static uint8_t buffer[BUFFER_LEN];

#define FOREACH_TEST(TEST) \
    TEST(invalid_key)
    /*TEST(valid)\
    TEST(invalid_object)\
    TEST(invalid_read)\
    TEST(invalid_digest_init)\
    TEST(invalid_digest_update)\*/


#define DECLARE_TEST(name) \
    void verify_object_##name (digest_func func);

FOREACH_TEST(DECLARE_TEST)

void verify_object_valid(digest_func func) {
    NO_MOCK(memory_open);
    NO_MOCK(memory_read);
    NO_MOCK(memory_close);
    NO_MOCK(get_size);
    NO_MOCK(get_offset);
    NO_MOCK(get_digest);

    mem_object obj_t;
    pull_error err = verify_object(OBJ_1, func, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE_MESSAGE(err == PULL_SUCCESS, err_as_str(err));
}

void verify_object_invalid_object(digest_func func) {
    mem_object obj_t;
    pull_error err;
    err = verify_object(42, func, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE(err);
}

void verify_object_invalid_read(digest_func func) {
    NO_MOCK(memory_open);
    memory_read_IgnoreAndReturn(MEMORY_READ_ERROR);
    NO_MOCK(memory_close);

    mem_object obj_t;
    pull_error err;
    err = verify_object(OBJ_1, func, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE(err == MEMORY_READ_ERROR);
}

void verify_object_invalid_digest_init(digest_func func) {
    NO_MOCK(memory_open);
    NO_MOCK(memory_read);
    NO_MOCK(memory_close);

    mem_object obj_t;
    pull_error err;
    func.init = invalid_init;
    err = verify_object(OBJ_1, func, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE(err == DIGEST_INIT_ERROR);
}

void verify_object_invalid_digest_update(digest_func func) {
    NO_MOCK(memory_open);
    NO_MOCK(memory_read);
    NO_MOCK(memory_close);
    NO_MOCK(get_size);
    NO_MOCK(get_offset);
    NO_MOCK(get_digest);

    mem_object obj_t;
    pull_error err;
    func.update = invalid_update;
    err = verify_object(OBJ_1, func, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE(err == DIGEST_UPDATE_ERROR);
}

void verify_object_invalid_key(digest_func func) {
    NO_MOCK(memory_open);
    NO_MOCK(memory_read);
    NO_MOCK(memory_close);
    NO_MOCK(get_size);
    NO_MOCK(get_offset);
    NO_MOCK(get_digest);
    NO_MOCK(verify_manifest_vendor);

    mem_object obj_t;
    pull_error err = verify_object(OBJ_1, func, y, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
    TEST_ASSERT_TRUE_MESSAGE(err == VERIFICATION_FAILED_ERROR, err_as_str(err));
}


