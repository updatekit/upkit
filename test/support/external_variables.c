/* This file is used to define the external variables
 * that must be defined to compile the library. In this case
 * will be used for the tests, but in a running version will
 * be obviously different
 */

/*** Memory configuration for tests */
const mem_slot_t memory_slots[] = {
    {
        .id = OBJ_RUN,
        .bootable = true,
        .loaded = true
    },
    {
        .id = OBJ_1,
        .bootable = false,
        .loaded = false
    },
    {
        .id = OBJ_2,
        .bootable = false,
        .loaded = false
    },
    {OBJ_END}
};

const version_t running_version = 0x0001;
