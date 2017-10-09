#ifndef _MEMORY_H
#define _MEMORY_H

#include "common/error.h"
#include "common/settings.h"
#include <stdint.h>

typedef struct mem_object_ mem_object;
typedef uint8_t obj_id;

// TODO This should be refactored to support dinamic memory. This means that this
// function should receive a mem_object** ctx, in this way it is able to
// set the pointer value if it allocates the memory
pull_error memory_open(mem_object* ctx, obj_id obj);

int memory_read(mem_object* ctx, void* memory_buffer, uint16_t size, uint32_t offset);

int memory_write(mem_object* ctx, const void* memory_buffer, uint16_t size, uint32_t offset);

pull_error memory_flush(mem_object* ctx);

pull_error memory_close(mem_object* ctx);

#endif // _MEMORY_H
