/** \file verifier.h
 * \author Antonio Langiu
 * \defgroup sec_ver
 * \brief This module includes component to perform secured updates.
 * 
 * The components allow to grant integrity, authenticity and confidentiality
 * during the update process. You can include this components in your update
 * runner to reach your application needs.
 * \{
 */
#ifndef LIBPULL_SECURITY_VERIFIER_H_
#define LIBPULL_SECURITY_VERIFIER_H_

#include <libpull/common.h>
#include <libpull/memory/memory_interface.h>
#include <libpull/security/digest.h>
#include <libpull/security/ecc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

pull_error verify_object(mem_object_t* obj, safestore_t sf, uint8_t* buffer, 
        size_t buffer_len);
        
pull_error verify_manifest(manifest_t* mt, safestore_t sf);

pull_error verify_digest(mem_object_t* obj, manifest_t* mt, uint8_t* buffer, size_t buffer_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_SECURITY_VERIFIER_H_ */
