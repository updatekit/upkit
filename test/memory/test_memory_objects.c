#include "common/libpull.h"
#include "memory/memory_objects.h"
#include "memory/manifest.h"
#include "memory_file_posix.h"

#include "test_runner.h"
#include "unity.h"
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

mem_object obj_run;
mem_object obj_1;
mem_object obj_2;
mem_object obj_t; // This is a temporary object used by the functions

void setUp(void) {
    TEST_ASSERT(memory_open(&obj_run, OBJ_RUN, WRITE_ALL) == PULL_SUCCESS);
    TEST_ASSERT(memory_open(&obj_1, OBJ_1, WRITE_ALL) == PULL_SUCCESS);
    TEST_ASSERT(memory_open(&obj_2, OBJ_2, WRITE_ALL) == PULL_SUCCESS);
}

void tearDown(void) {
    TEST_ASSERT(memory_close(&obj_run) == PULL_SUCCESS);
    TEST_ASSERT(memory_close(&obj_1) == PULL_SUCCESS);
    TEST_ASSERT(memory_close(&obj_2) == PULL_SUCCESS);
}

void test_get_newest_firmware(void) {
    obj_id newest = 0;
    version_t version = 0;
    pull_error err = get_newest_firmware(&newest, &version, &obj_t);
    TEST_ASSERT_TRUE(!err);
    TEST_ASSERT_EQUAL_HEX16(version, 0xbeef);
    TEST_ASSERT(newest == OBJ_1);
}

void test_get_oldest_slot(void) {
    obj_id oldest;
    version_t version = 0;
    pull_error err = get_oldest_firmware(&oldest, &version, &obj_t);
    TEST_ASSERT_TRUE(!err);
    TEST_ASSERT_EQUAL_HEX16(0x0, version);
    TEST_ASSERT_EQUAL_INT(OBJ_2, oldest);
}

void test_read_slot_manifest(void) {
    manifest_t mt;
    pull_error err = read_firmware_manifest(&obj_1, &mt);
    TEST_ASSERT_TRUE(!err);
    TEST_ASSERT_EQUAL_HEX16(0xbeef, mt.vendor.version);
}

void test_write_slot_manifest(void) {
    manifest_t mt_old, mt_new;
    version_t version;
    obj_id newest;
    pull_error err;
    err = read_firmware_manifest(&obj_2, &mt_old);
    TEST_ASSERT_TRUE(!err);
    mt_new.vendor.version = 0xffff;
    err = write_firmware_manifest(&obj_2, &mt_new);
    TEST_ASSERT_TRUE(!err);
    get_newest_firmware(&newest, &version, &obj_t);
    TEST_ASSERT_EQUAL_INT8(OBJ_2, newest);
    err = write_firmware_manifest(&obj_2, &mt_old);
    TEST_ASSERT_TRUE(!err);
}

int file_compare(char* fname1, char* fname2) {
	FILE *fp1, *fp2;
	int ch1, ch2;
	fp1 = fopen(fname1, "r");
	fp2 = fopen(fname2, "r");
	if (fp1 == NULL) {
		printf("Cannot open %s for reading\n", fname1);
		exit(1);
	} else if (fp2 == NULL) {
		printf("Cannot open %s for reading\n", fname2);
		exit(1);
	}

	do {
		ch1 = getc(fp1);
		ch2 = getc(fp2);
	} while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2));
	fclose(fp1);
	fclose(fp2);
	if (ch1 == ch2) {
		return 0;
	} 
	return 1;
}

void test_copy_firmware(void) {
    uint8_t buffer[BUFFER_SIZE];
	pull_error err = copy_firmware(&obj_1, &obj_run, buffer, BUFFER_SIZE);
	TEST_ASSERT_TRUE_MESSAGE(!err, err_as_str(err));
	TEST_ASSERT(file_compare("../assets/expected_internal_flash_simulator", 
    				"../assets/internal_flash_simulator") == 0);
}
