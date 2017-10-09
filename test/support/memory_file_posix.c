#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "common/logger.h"
#include "common/settings.h"

#include "memory_file_posix.h"

const int8_t memory_objects[] = { OBJ_1, OBJ_2, OBJ_END};

char* memory_objects_mapper[] = {
    [OBJ_GOLD] = "antani",
    [OBJ_RUN] = "assets/internal_flash_simulator",
    [OBJ_1] = "assets/external_flash_simulator",
    [OBJ_2] = "assets/external_flash_simulator",
    [TEST_MEMORY_FILE] = "assets/test_memory",
};
int memory_objects_start[] = {
    [OBJ_GOLD] = 0,
    [OBJ_RUN] = 0x7000,
    [OBJ_1] = 0x19000,
    [OBJ_2] = 0x32000,
    [TEST_MEMORY_FILE] = 0
};
int memory_objects_end[] = {
    [OBJ_GOLD] = 0,
    [OBJ_RUN] = 0x20000,
    [OBJ_1] = 0x32000,
    [OBJ_2] = 0x4B000,
    [TEST_MEMORY_FILE] = 0x19000
};

/******* Testing Function ******/
void override_memory_object(obj_id id, char* path, int start, int end) {
    memory_objects_mapper[id] = path;
    memory_objects_start[id] = start;
    memory_objects_end[id] = end;
}
/**** End Testing Function *****/

pull_error resource_mapper(mem_object* ctx, obj_id obj) {
    if (obj <= OBJ_FIRST || obj >= OBJ_LAST) {
        return INVALID_OBJECT_ERROR;
    }
    ctx->path = memory_objects_mapper[obj];
    ctx->start_offset = memory_objects_start[obj];
    ctx->end_offset = memory_objects_end[obj];
    return PULL_SUCCESS;
}

pull_error memory_open_impl(mem_object* ctx, obj_id obj) {
    bzero(ctx, sizeof(mem_object));
    if (resource_mapper(ctx, obj) != PULL_SUCCESS) {
        log_error(MEMORY_MAPPING_ERROR, "Error mapping the resource\n");
        return MEMORY_MAPPING_ERROR;
    }
    ctx->fp = open(ctx->path, O_RDWR | O_CREAT);
    if (ctx->fp < 0) {
        log_error(MEMORY_OPEN_ERROR, "Impossible to open the file %s\n", ctx->path);
        return MEMORY_OPEN_ERROR;
    }
    return PULL_SUCCESS;
}

int memory_read_impl(mem_object* ctx, void* memory_buffer, uint16_t size, uint32_t offset) {
    assert(ctx != NULL);
    assert(ctx->fp >= 0);
    if (ctx->start_offset+offset+size > ctx->end_offset) {
        log_error(INVALID_ACCESS_ERROR, "Invalid access\n");
        return -1;
    }
    if (lseek(ctx->fp, ctx->start_offset+offset, SEEK_SET) < 0) {
        log_error(MEMORY_READ_ERROR, "Impossible to set the file position: %s\n", strerror(errno));
        return -1;
    }
    return read(ctx->fp, memory_buffer, size);

}

int memory_write_impl(mem_object* ctx, const void* memory_buffer, uint16_t size, uint32_t offset) {
    assert(ctx != NULL);
    assert(ctx->fp > 0);
    if (ctx->start_offset+offset+size > ctx->end_offset) {
        log_error(INVALID_ACCESS_ERROR, "Invalid access\n");
        return -1;
    }
    if (lseek(ctx->fp, ctx->start_offset+offset, SEEK_SET) < 0) {
        log_error(MEMORY_WRITE_ERROR, "Impossible to set the file position: %s\n", strerror(errno));
        return -1;
    }
    return write(ctx->fp, memory_buffer, size);
}

pull_error memory_flush_impl(mem_object* ctx) {
    // not implemented since there is no buffer
    return PULL_SUCCESS;
}

pull_error memory_close_impl(mem_object* ctx) {
    if (close(ctx->fp) != 0) {
        return MEMORY_CLOSE_ERROR;
    }
    return PULL_SUCCESS;
}
