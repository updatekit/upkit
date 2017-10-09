#ifndef _MEMORY_OBJECTS_H
#define _MEMORY_OBJECTS_H

#include <stdio.h>
#include <stdint.h>
#include "common/error.h"
#include "memory/memory.h"
#include "memory/metadata.h"
#include "common/settings.h"

/* The mem_object type is not defined in the library and must
 * be defined by the memory implementation used to the library.
 * For this reason every time a function that requires to work
 * with memory is called, it must be also passed to it a
 * an allocated memory object. This can be done allocating it
 * statically or dinamically, depending on the platform constraints
 */

pull_error get_newest_firmware(obj_id* obj, uint16_t* version, mem_object* obj_t);

pull_error get_oldest_firmware(obj_id* obj, uint16_t* version, mem_object* obj_t);

pull_error copy_firmware(obj_id s, obj_id d, mem_object* src, mem_object* dst);

pull_error read_firmware_metadata(obj_id o, metadata* mt, mem_object* obj);

pull_error write_firmware_metadata(obj_id o, const metadata* mt, mem_object* obj);

#endif // _MEMORY_OBJECTS_H
