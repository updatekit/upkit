#include <libpull/common.h>
#include <libpull/memory.h>

#include "support/support.h"

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

mem_object_t obj_a;
mem_object_t obj_b;
mem_object_t obj_c;
mem_object_t obj_t; // This is a temporary object used by the functions

void ntest_prepare(void) {
    restore_assets();
    nTEST_TRUE(memory_open(&obj_a, OBJ_A, WRITE_ALL) == PULL_SUCCESS);
    nTEST_TRUE(memory_open(&obj_b, OBJ_B, WRITE_ALL) == PULL_SUCCESS);
    nTEST_TRUE(memory_open(&obj_c, OBJ_C, WRITE_ALL) == PULL_SUCCESS);
}

void ntest_clean(void) {
    nTEST_TRUE(memory_close(&obj_a) == PULL_SUCCESS);
    nTEST_TRUE(memory_close(&obj_b) == PULL_SUCCESS);
    nTEST_TRUE(memory_close(&obj_c) == PULL_SUCCESS);
    restore_assets();
}

void test_get_newest_firmware(void) {
    mem_id_t newest = 0;
    version_t version = 0;
    pull_error err = get_newest_firmware(&newest, &version, &obj_t, false, false);
    nTEST_TRUE(!err);
    nTEST_COMPARE_HEX(0xc, version);
    nTEST_COMPARE_INT(OBJ_C, newest);
}

void test_get_oldest_slot(void) {
    mem_id_t oldest;
    version_t version = 0;
    pull_error err = get_oldest_firmware(&oldest, &version, &obj_t, false, false);
    nTEST_TRUE(!err);
    nTEST_COMPARE_HEX(0xa, version);
    nTEST_COMPARE_INT(OBJ_A, oldest);
}

void test_read_slot_manifest(void) {
    manifest_t mt;
    pull_error err = read_firmware_manifest(&obj_a, &mt);
    nTEST_TRUE(!err);
    nTEST_COMPARE_HEX(0xa, get_version(&mt));
}

void test_write_slot_manifest(void) {
    manifest_t mt_old, mt_new;
    version_t version;
    mem_id_t newest;
    pull_error err;
    err = read_firmware_manifest(&obj_a, &mt_old);
    nTEST_TRUE(!err);
    set_version(&mt_new, 0xffff);
    err = write_firmware_manifest(&obj_a, &mt_new);
    nTEST_TRUE(!err);
    get_newest_firmware(&newest, &version, &obj_t, false, false);
    nTEST_COMPARE_INT(OBJ_A, newest);
    nTEST_COMPARE_HEX(0xffff, version);
    err = write_firmware_manifest(&obj_a, &mt_old);
    nTEST_TRUE(!err);
}

void test_copy_firmware(void) {
    uint8_t buffer[BUFFER_SIZE];
	pull_error err = copy_firmware(&obj_a, &obj_b, buffer, BUFFER_SIZE);
	nTEST_TRUE(!err, "%s\n", err_as_str(err));
	nTEST_TRUE(file_compare("../assets/slot_a.bin", 
    				"../assets/slot_b.bin") == 0);
}

void test_swap_firmware(void) {
    uint8_t buffer[BUFFER_SIZE];
    pull_error err = swap_slots(&obj_a, &obj_b, &obj_c, BUFFER_SIZE*4, buffer, BUFFER_SIZE); 
    nTEST_TRUE(!err, "%s\n", err_as_str(err));
    nTEST_TRUE(file_compare("../assets/slot_a.bin", "../assets/slot_b.pristine") == 0);
    nTEST_TRUE(file_compare("../assets/slot_b.bin", "../assets/slot_a.pristine") == 0);
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_get_newest_firmware);
    nTEST_RUN(test_get_oldest_slot);
    nTEST_RUN(test_read_slot_manifest);
    nTEST_RUN(test_write_slot_manifest);
    nTEST_RUN(test_copy_firmware);
    nTEST_RUN(test_swap_firmware);
    nTEST_END();
    nTEST_RETURN();
}
