/** \file loader.h
 * \author Antonio Langiu
 * \defgroup com_loader
 * \{
 */
#ifndef LIBPULL_COMMON_LOADER_H_
#define LIBPULL_COMMON_LOADER_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** 
 * \brief Loader interface used to load the object once stored.
 * 
 * \param address address to be loaded
 */
void load_object(address_t address);

#ifdef __cplusplus
 }
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_LOADER_H_ */
