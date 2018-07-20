#include <libpull/memory.h>
#include <libpull/common.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

pull_error get_ordered_firmware(mem_id_t* id, version_t* version, mem_object_t* obj_t, bool newest,
        bool disable_running, bool prefer_bootable) {
    manifest_t mt;
    pull_error err;
    mem_id_t i;
    bool first = true;
    bool bootable;
    for (i=0; memory_slots[i].id > 0; i++) {
        err = memory_open(obj_t, memory_slots[i].id, READ_ONLY);
        if (err) {
            log_error(err, "Failure opening firmware id %d\n", memory_slots[i].id);
            return GET_NEWEST_ERROR;
        }
        err = read_firmware_manifest(obj_t, &mt);
        if (err) {
            log_error(err, "Failure reading firmware manifest for object %d\n", memory_slots[i].id);
            return GET_NEWEST_ERROR;
        }
        memory_close(obj_t);
        // Avoid overriding the slot with the running image
        if (disable_running && memory_slots[i].loaded == true) {
            continue;
        }
        if (first) {
            *version = mt.vendor.version;
            *id = memory_slots[i].id;
            bootable = memory_slots[i].bootable;
            first = false;
        } else {
            if (prefer_bootable && (bootable && !memory_slots[i].bootable)) {
                continue;
            }
            if (newest == true) {
                if (mt.vendor.version > *version) {
                    *version = mt.vendor.version;
                    *id = memory_slots[i].id; // XXX This needs to be fixed
                }
            } else {
                if (mt.vendor.version < *version) {
                    *version= mt.vendor.version;
                    *id = memory_slots[i].id; // This needs to be fixed XXX
                }
            }
        }
    }
    return PULL_SUCCESS;
}

pull_error get_newest_firmware(mem_id_t* id, version_t* version, 
        mem_object_t* obj_t, bool disable_running, bool prefer_bootable) {
    return get_ordered_firmware(id, version, obj_t, true, disable_running, prefer_bootable);
}

pull_error get_oldest_firmware(mem_id_t* id, version_t* version, 
        mem_object_t* obj_t, bool disable_running, bool prefer_bootable) {
    return get_ordered_firmware(id, version, obj_t, false, disable_running, prefer_bootable);
}

pull_error copy_firmware(mem_object_t* src, mem_object_t* dst, uint8_t* buffer, size_t buffer_size) {
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

static pull_error swap_chunk(mem_object_t* src, mem_object_t* dst, address_t offset, address_t swap_size, uint8_t* buffer, size_t buffer_size, bool read_swap, bool write_swap) {
    size_t step = buffer_size;
    int swap_full = 0;
    int done;
    while (swap_full < swap_size) {
        if (swap_full + step > swap_size) {
            step = swap_size - swap_full;
        }
        done = memory_read(src, buffer, step, (read_swap)? swap_full: offset);
        if (done != step) {
            log_error(MEMORY_READ_ERROR, "Error reading from obj1\n");
            return MEMORY_READ_ERROR;
        }
        done = memory_write(dst, buffer, step, (write_swap)? swap_full: offset);
        if (done != step) {
            log_error(MEMORY_WRITE_ERROR, "Error writing the swap object\n");
            return MEMORY_WRITE_ERROR;
        }
        swap_full += step;
        offset += step;
    }
    return PULL_SUCCESS;
}

pull_error swap_slots(mem_object_t* obj1, mem_object_t* obj2, mem_object_t* obj_swap,
                        size_t swap_size, uint8_t* buffer, size_t buffer_size) {
    // This function assumes that the size of the swap partition is a multiple
    // of the buffer size
    PULL_ASSERT(swap_size > buffer_size && swap_size % buffer_size == 0);
    address_t size, size2;
    manifest_t mt;
    pull_error err;
    // Get size1
    err = read_firmware_manifest(obj1, &mt);
    if (err) {
        return err;
    }
    size = get_size(&mt) + get_offset(&mt);
    // Get size2
    err = read_firmware_manifest(obj2, &mt);
    if (err) {
        return err;
    }
    size2 = get_size(&mt) + get_offset(&mt);
    // Compare size
    if (size2 > size) {
        size = size2;
    }
    // Start swapping object
    int offset = 0;
    while (offset < size) {
        if (offset + swap_size > size) {
            swap_size = size - offset;
        }
        // Read obj1 into obj_swap until you fill the swap_size
        err = swap_chunk(obj1, obj_swap, offset, swap_size, buffer, buffer_size, false, true);
        if (err) {
            log_error(err, "Error reading obj1 into obj_swap\n");
            return err;
        }
        // Move obj2 chunk into obj1
        swap_chunk(obj2, obj1, offset, swap_size, buffer, buffer_size, false, false);
        if (err) {
            log_error(err, "Error reading obj2 into obj1\n");
            return err;
        }
        // Move obj_swap chunk into obj2
        swap_chunk(obj_swap, obj2, offset, swap_size, buffer, buffer_size, true, false);
        if (err) {
            log_error(err, "Error reading obj_swap into obj2\n");
            return err;
        }
        // Incremente the offset
        offset += swap_size;
    }
    return PULL_SUCCESS;
}

pull_error read_firmware_manifest(mem_object_t* obj, manifest_t* mt) {
    if (memory_read(obj, (uint8_t*) mt, sizeof(manifest_t), 0) != sizeof(manifest_t)) {
        log_error(MEMORY_READ_ERROR, "Failure reading object\n");
        return READ_MANIFEST_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error write_firmware_manifest(mem_object_t* obj, const manifest_t* mt) {
    if (memory_write(obj, (uint8_t*) mt, sizeof(manifest_t), 0) != sizeof(manifest_t)) {
        memory_close(obj);
        log_error(MEMORY_WRITE_ERROR, "Failure writing manifest into object\n");
        return WRITE_MANIFEST_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error invalidate_object(mem_id_t id, mem_object_t* obj) {
    pull_error err = memory_open(obj, id, WRITE_CHUNK) != PULL_SUCCESS;
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
