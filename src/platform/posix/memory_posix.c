#ifdef WITH_PLATFORM_POSIX

#include "memory_posix.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

const mem_slot_t memory_slots[] = {
    {
        .id = OBJ_A,
        .bootable = true,
        .loaded = true
    },
    {
        .id = OBJ_B,
        .bootable = true,
        .loaded = false
    },
    {
        .id = OBJ_C,
        .bootable = false,
        .loaded = false
    },
    {OBJ_END}
};

char* memory_objects_mapper[] = {
    [OBJ_GOLD] = "antani",
    [BOOTLOADER_CTX] = "../assets/bootctx",
    [OBJ_A] = "../assets/slot_a.bin",
    [OBJ_B] = "../assets/slot_b.bin",
    [OBJ_C] = "../assets/slot_c.bin",
    [TEST_MEMORY_FILE] = "../assets/test_memory",
};

/******* Testing Function ******/
#ifndef DISABLE_TESTING
static char* memory_object_pristine[] = {
    [BOOTLOADER_CTX] = "../assets/bootctx.pristine",
    [OBJ_A] = "../assets/slot_a.pristine",
    [OBJ_B] = "../assets/slot_b.pristine",
    [OBJ_C] = "../assets/slot_c.pristine",
};

void copy_asset(char* src, char* dest) {
    FILE *s, *d;
    int ch;
    s = fopen(src, "r");
    if (s == NULL) {
        log_warn("Cannot open src file %s\n", src);
        return;
    }
    d = fopen(dest, "w");
    if (d == NULL) {
        fclose(s);
        log_warn("Cannot open dest file %s\n", dest);
        return;
    }
    while ( (ch = fgetc(s)) != EOF) {
        fputc(ch, d);
    }
    fclose(s);
    fclose(d);
}

void restore_assets() {
    int index = 0;
    mem_id_t id;
    for (index = 0; memory_slots[index].id != OBJ_END; index++) {
        id = memory_slots[index].id;
        copy_asset(memory_object_pristine[id], memory_objects_mapper[id]);
    }
    copy_asset(memory_object_pristine[BOOTLOADER_CTX], memory_objects_mapper[BOOTLOADER_CTX]);
}
#endif /* DISABLE_TESTING */
/**** End Testing Function *****/

pull_error resource_mapper(mem_object_t* ctx, mem_id_t id) {
    if (id <= OBJ_FIRST || id >= OBJ_LAST) {
        return INVALID_OBJECT_ERROR;
    }
    ctx->path = memory_objects_mapper[id];
    return PULL_SUCCESS;
}

pull_error memory_open_impl(mem_object_t* ctx, mem_id_t id, mem_mode_t mode) {
    bzero(ctx, sizeof(mem_object_t));
    if (resource_mapper(ctx, id) != PULL_SUCCESS) {
        log_error(MEMORY_MAPPING_ERROR, "Error mapping the resource\n");
        return MEMORY_MAPPING_ERROR;
    }
    // TODO analyze how to map the memory mode
    ctx->fp = open(ctx->path, O_RDWR | O_CREAT, 0644);
    if (ctx->fp < 0) {
        log_error(MEMORY_OPEN_ERROR, "Impossible to open the file %s\n", ctx->path);
        return MEMORY_OPEN_ERROR;
    }
    return PULL_SUCCESS;
}

int memory_read_impl(mem_object_t* ctx, void* memory_buffer, size_t size, address_t offset) {
    PULL_ASSERT(ctx != NULL);
    PULL_ASSERT(ctx->fp >= 0);
    if (lseek(ctx->fp, offset, SEEK_SET) < 0) {
        log_error(MEMORY_READ_ERROR, "Impossible to set the file position: %s\n", strerror(errno));
        return -1;
    }
    return read(ctx->fp, memory_buffer, size);
}

int memory_write_impl(mem_object_t* ctx, const void* memory_buffer, size_t size, address_t offset) {
    PULL_ASSERT(ctx != NULL);
    PULL_ASSERT(ctx->fp > 0);
    if (lseek(ctx->fp, offset, SEEK_SET) < 0) {
        log_error(MEMORY_WRITE_ERROR, "Impossible to set the file position: %s\n", strerror(errno));
        return -1;
    }
    return write(ctx->fp, memory_buffer, size);
}

pull_error memory_close_impl(mem_object_t* ctx) {
    if (ctx == NULL) {
        return MEMORY_CLOSE_ERROR;
    }
    if (close(ctx->fp) != 0) {
        return MEMORY_CLOSE_ERROR;
    }
    return PULL_SUCCESS;
}

#endif /* WITH_PLATFORM_POSIX */
