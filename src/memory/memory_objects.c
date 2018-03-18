#include "memory/memory_objects.h"
#include "memory/memory.h"
#include "memory/manifest.h"
#include <string.h>
#include <stdio.h>

// if not newest, then is oldest
pull_error get_ordered_firmware(obj_id* obj, version_t* version, mem_object* obj_t, bool newest) {
    manifest_t mt;
    pull_error err;
    obj_id i;
    for (i=0; memory_objects[i] > 0; i++) {
        err = memory_open(obj_t, memory_objects[i], READ_ONLY);
        if (err) {
            log_error(err, "Failure opening firmware id %d\n", memory_objects[i]);
            return GET_NEWEST_ERROR;
        }
        err = read_firmware_manifest(obj_t, &mt);
        if (err) {
            log_error(err, "Failure reading firmware manifest for object %d\n", memory_objects[i]);
            return GET_NEWEST_ERROR;
        }
        memory_close(obj_t);
        if (i == 0) {
            *version = mt.vendor.version;
            *obj = memory_objects[i];
        } else {
            if (newest == true) {
                if (mt.vendor.version > *version) {
                    *version = mt.vendor.version;
                    *obj = memory_objects[i];
                }
            } else {
                if (mt.vendor.version < *version) {
                *version= mt.vendor.version;
                *obj = memory_objects[i];
                }
            }
        }
    }
    return PULL_SUCCESS;
}

pull_error get_newest_firmware(obj_id* obj, version_t* version, mem_object* obj_t) {
    return get_ordered_firmware(obj, version, obj_t, true);
}

pull_error get_oldest_firmware(obj_id* obj, version_t* version, mem_object* obj_t) {
    return get_ordered_firmware(obj, version, obj_t, false);
}

pull_error copy_firmware(mem_object* src, mem_object* dst, uint8_t* buffer, size_t buffer_size) {
    manifest_t srcmt;
    int firmware_size = 0;
    if (read_firmware_manifest(src, &srcmt)) {
        log_error(COPY_FIRMWARE_ERROR, "Error reading source manifest\n");
        return COPY_FIRMWARE_ERROR;
    }
    firmware_size = get_size(&srcmt) + get_offset(&srcmt);
    log_debug("Firmware size is %d\n", firmware_size);

    int offset = 0, step = buffer_size, readed = 0;
    while (offset < firmware_size && 
            (readed = memory_read(src, buffer, step, offset)) > 0) {
        if (memory_write(dst, buffer, readed, offset) != readed) {
            log_error(COPY_FIRMWARE_ERROR, "Error writing to destination object");
            return COPY_FIRMWARE_ERROR;
        }
        offset+=readed;
        if (offset + step > firmware_size) {
            step = firmware_size - offset;
        }
    }
    if (readed <= 0) {
        log_error(COPY_FIRMWARE_ERROR, "Error while copying firmware\n");
        return COPY_FIRMWARE_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error read_firmware_manifest(mem_object* obj, manifest_t* mt) {
    if (memory_read(obj, (uint8_t*) mt, sizeof(manifest_t), 0) != sizeof(manifest_t)) {
        log_error(MEMORY_READ_ERROR, "Failure reading object\n");
        return READ_MANIFEST_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error write_firmware_manifest(mem_object* obj, const manifest_t* mt) {
    if (memory_write(obj, (uint8_t*) mt, sizeof(manifest_t), 0) != sizeof(manifest_t)) {
        memory_close(obj);
        log_error(MEMORY_WRITE_ERROR, "Failure writing manifest into object\n");
        return WRITE_MANIFEST_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error invalidate_object(obj_id id, mem_object* obj) {
    pull_error err = memory_open(obj, id, WRITE_ALL) != PULL_SUCCESS;
    if (err) {
        log_error(err, "Failure opening firmware\n");
        return WRITE_MANIFEST_ERROR;
    }
    manifest_t nullmt;
    memset(&nullmt, 0, sizeof(manifest_t));
    err = write_firmware_manifest(obj, &nullmt);
    if (err) {
        log_error(err, "Failure writing firmware manifest on object%d\n", id);
        return INVALIDATE_OBJECT_ERROR;
    }
    memory_close(obj);
    return PULL_SUCCESS;
}
