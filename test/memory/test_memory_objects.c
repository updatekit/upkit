#include <libpull/common.h>
#include <libpull/memory.h>

#include "support/support.h"

#include <stdio.h>
#include <stdlib.h>

#define FOREACH_TEST(DO) \
    DO(get_newest_firmware, 0) \
    DO(get_oldest_slot, 0) \
    DO(read_slot_manifest, 0) \
    DO(write_slot_manifest, 0) \
    DO(copy_firmware, 0)

TEST_RUNNER();

#define BUFFER_SIZE 1024

mem_object_t obj_a;
mem_object_t obj_b;
mem_object_t obj_c;
mem_object_t obj_d;
mem_object_t obj_t; // This is a temporary object used by the functions

void setUp(void) {
    TEST_ASSERT(memory_open(&obj_a, OBJ_A, WRITE_ALL) == PULL_SUCCESS);
    TEST_ASSERT(memory_open(&obj_b, OBJ_B, WRITE_ALL) == PULL_SUCCESS);
}

void tearDown(void) {
    TEST_ASSERT(memory_close(&obj_a) == PULL_SUCCESS);
    TEST_ASSERT(memory_close(&obj_b) == PULL_SUCCESS);
}

void test_get_newest_firmware(void) {
    mem_id_t newest = 0;
    version_t version = 0;
    pull_error err = get_newest_firmware(&newest, &version, &obj_t, false, false);
    TEST_ASSERT_TRUE(!err);
    TEST_ASSERT_EQUAL_HEX16(0xc, version);
    TEST_ASSERT_EQUAL_INT(OBJ_C, newest);
}

void test_get_oldest_slot(void) {
    mem_id_t oldest;
    version_t version = 0;
    pull_error err = get_oldest_firmware(&oldest, &version, &obj_t, false, false);
    TEST_ASSERT_TRUE(!err);
    TEST_ASSERT_EQUAL_HEX16(0xa, version);
    TEST_ASSERT_EQUAL_INT(OBJ_A, oldest);
}

void test_read_slot_manifest(void) {
    manifest_t mt;
    pull_error err = read_firmware_manifest(&obj_a, &mt);
    TEST_ASSERT_TRUE(!err);
    TEST_ASSERT_EQUAL_HEX16(0xa, get_version(&mt));
}

void test_write_slot_manifest(void) {
    manifest_t mt_old, mt_new;
    version_t version;
    mem_id_t newest;
    pull_error err;
    err = read_firmware_manifest(&obj_a, &mt_old);
    TEST_ASSERT_TRUE(!err);
    set_version(&mt_new, 0xffff);
    err = write_firmware_manifest(&obj_a, &mt_new);
    TEST_ASSERT_TRUE(!err);
    get_newest_firmware(&newest, &version, &obj_t, false, false);
    TEST_ASSERT_EQUAL_INT8(OBJ_A, newest);
    TEST_ASSERT_EQUAL_HEX(0xffff, version);
    err = write_firmware_manifest(&obj_a, &mt_old);
    TEST_ASSERT_TRUE(!err);
}

void test_copy_firmware(void) {
    uint8_t buffer[BUFFER_SIZE];
	pull_error err = copy_firmware(&obj_a, &obj_b, buffer, BUFFER_SIZE);
	TEST_ASSERT_TRUE_MESSAGE(!err, err_as_str(err));
	TEST_ASSERT_TRUE(file_compare("../assets/slot_a.bin", 
    				"../assets/slot_b.bin") == 0);
}
