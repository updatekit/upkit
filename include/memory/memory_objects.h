/** \file memory_objects.h
 * \brief This file contains function to work with the memory object
 * abastrction.
 * \author Antonio Langiu
 * \defgroup Memory
 * \brief This module include components to interact with a memory object.
 *
 * It rapresent an abstraction allowing to intract with a memory object
 * without having to know the underlying structure. In fact it relies 
 * on the memory interface implemented by the developer using the library.
 * \{
 */
#ifndef MEMORY_OBJECTS_H_
#define MEMORY_OBJECTS_H_

#include <common/libpull.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief Get the id of the memory object containing the newest firmware.
 *
 * \param[out] id The id of the newest object.
 * \param[out] version The version of the newest object.
 * \param[in] obj_t A temporary mem_object used by the function.
 * \returns PULL_SUCCESS on success or a specific error otherwise.
 */
pull_error get_newest_firmware(obj_id *id, uint16_t *version,
                               mem_object *obj_t);

/**
 * \brief Get the id of the memory object containing the oldest firmware.
 *
 * \param[out] obj The id of the oldest object
 * \param[out] version The version of the oldest object.
 * \param[in] obj_t A temporary mem_object used by the function.
 * \returns PULL_SUCCESS on success or a specific error otherwise.
 */
pull_error get_oldest_firmware(obj_id *obj, uint16_t *version,
                               mem_object *obj_t);

/**
 * \brief Copy the firmware s into the firmware d.
 *
 * This function will use the size specified in the s firmware
 * manifest to correcly copy the firmware.
 * \param s
 * \param d
 * \param src A temporary mem_object used by the function.
 * \param dst A temporary mem_object used by the function.
 * \param buffer Buffer used to copy the object
 * \param buffer_size The size of the buffer
 * 
 * \note The buffer will be used to read from object s and to write
 * to obejct d. If you are working with flash and your memory implementation
 * is not buffered you can pass a buffer with size equal to the size of a flash
 * page.
 *
 * \returns PULL_SUCCESS on success or a specific error otherwise.
 */
pull_error copy_firmware(obj_id s, obj_id d, mem_object *src, mem_object *dst, uint8_t* buffer, size_t buffer_size);

/**
 * \brief Read the manifest of the memory object.
 *
 * This function will use the size specified in the s firmware
 * manifest to correcly copy the firmware.
 * \param[in] id Id of the object.
 * \param[out] mt manifest of the memory object.
 * \param obj_t A temporary mem_object used by the function.
 * \returns PULL_SUCCESS on success or a specific error otherwise.
 */
pull_error read_firmware_manifest(obj_id id, manifest_t *mt, mem_object *obj_t);

/**
 * \brief Write the manifest into the memory object.
 *
 * \param[in] id The id of the object.
 * \param[in] mt The manifest to be written.
 * \param obj_t A temporary mem_object used by the function.
 * \returns PULL_SUCCESS on success or a specific error otherwise.
 */
pull_error write_firmware_manifest(obj_id id, const manifest_t *mt,
                                   mem_object *obj_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} MEMORY_OBJECTS_H_ */
