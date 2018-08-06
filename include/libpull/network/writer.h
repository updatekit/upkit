/** \file writer.h
 * \brief Module used to write the received update to memory using a buffered
 * or not buffer output.
 * \author Antonio Langiu
 * \defgroup net_writer
 * \{
 */
#ifndef LIBPULL_NETWORK_WRITER_H_
#define LIBPULL_NETWORK_WRITER_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libpull/common.h>
#include <libpull/memory.h>
#include <string.h>

typedef pull_error (* validate_mt)(manifest_t* mt, void* user_data);

#define WRITER_BUFFER_LEN 4096

typedef struct writer_ctx_t {
    mem_object_t* obj;
    manifest_t mt;
    uint32_t received;
    uint32_t expected;
    bool manifest_received;
    validate_mt validate_cb;
    void* user_data;
#ifdef BUFFERED_WRITER
    uint8_t buffer[WRITER_BUFFER_LEN];
    uint16_t buffer_full;
    uint32_t buffer_ptr;
#endif
} writer_ctx_t;


pull_error writer_open(writer_ctx_t* ctx, mem_object_t* obj, validate_mt cb, void* user_data);
pull_error writer_chunk(writer_ctx_t* ctx, const char* data, uint32_t len);
pull_error writer_close(writer_ctx_t* ctx);

#ifdef __cplusplus
 }
#endif /* __cplusplus */
#endif /* \} LIBPULL_NETWORK_WRITER_H_ */
