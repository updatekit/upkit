/** \file memory.h
 * \brief Low level interface to access memory.
 *
 * The implementation is mandatory as it is used by the library to
 * interact with the memory of the device.
 * 
 * The mem_object_t type is not defined in the library and must
 * be defined by the memory implementation used to the library.
 * For this reason every time a function that requires to work
 * with memory is called, it must be also passed to it a
 * an allocated memory object. This can be done allocating it
 * statically or dinamically, depending on the platform uonstraints.
 * \author Antonio Langiu
 * \defgroup Memory
 * \{
 */
#ifndef MEMORY_H_
#define MEMORY_H_

#include <common/error.h>
#include <common/types.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct mem_object_t mem_object_t;

typedef struct mem_slot_t {
    mem_id_t id;
    bool bootable;
    bool loaded;
} mem_slot_t;

typedef enum {
    READ_ONLY = 0,
    WRITE_CHUNK = 1, // In this mode the memory will not be completely rewritten.
    WRITE_ALL = 2 // In this mode the memory object will be completely erased before.
} mem_mode_t;

/** 
 * \brief Open a memory object.
 * 
 * The implementation of this function should open a memory object given the
 * id. This can be mapped to any phisical object, such as a file, a ROM or
 * even an allocated memory object. It depends on the needs of the platform
 * and the application.
 * \param ctx An unitialized memory object
 * \param id The id of the memory object. The obj_id enum must be defined
 * when implementing this interface.
 * \param mode The mode used to open the memory_object (i.e. READ, WRITE, etc)
 * \returns PULL_SUCCESS if the memory was correctly open or the specific
 * erro 
 */
pull_error memory_open(mem_object_t* ctx, mem_id_t id, mem_mode_t mode);

/** 
 * \brief Read bytes from a memory object.
 * 
 * This function reads size bytes from a memory object at the specified offset
 * into the given memory buffer.
 * \param ctx The already opened memory object.
 * \param memory_buffer The memory buffer acting as destination.
 * \param size The number of bytes to read.
 * \param offset The offset in the memory object from where to start reading.
 * \returns The number of readed bytes or a negative number in case of error.
 */
int memory_read(mem_object_t* ctx, void* memory_buffer, size_t size, address_t offset);

/** 
 * \brief Write bytes into a memory object.
 * 
 * This function writes size bytes into an opened memory object at the offset
 * specified.
 * \param ctx An opened memory object.
 * \param memory_buffer The memory buffer to be written.
 * \param size The size of the memory buffer.
 * \param offset The offset into the memory object.
 * \returns The number of written bytes or a negative number in case of error.
 */
int memory_write(mem_object_t* ctx, const void* memory_buffer, size_t size, address_t offset);

/** 
 * \brief Flush the memory object. (Not mandatory)
 * 
 * This function must not always be implemented. In fact it should be
 * implemented only if the underlying implementation includes some
 * buffered I/O. In that case could be useful to flush the buffer.
 * The flush function is not direcly used by the library but may
 * be useful in some situations.
 * \param ctx The memory object containing the buffer to be flushed.
 * \returns PULL_SUCCESS if the buffer was correcly flushed or the
 * specific error otherwise.
 */
pull_error memory_flush(mem_object_t* ctx);

/** 
 * \brief Close the memory object.
 * 
 * This should close and deallocate all the initialized resources.
 * \param ctx The memory object.
 * \returns PULL_SUCCESS on success or a specific error otherwise.
 */
pull_error memory_close(mem_object_t* ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} MEMORY_H_ */
