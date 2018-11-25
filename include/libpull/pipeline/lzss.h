/** \file lzss.h
 * \author Antonio Langiu
 * \defgroup pip_lzss
 * \{
 */
#ifndef LIBPULL_PIPELINE_LZSS_DECODER_H_
#define LIBPULL_PIPELINE_LZSS_DECODER_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** This module is part of the libpull pipeline.
 * The decompressor has been adapted from the public domain code
 * of Haruhiko Okumura, that can be found at the website:
 * https://oku.edu.mie-u.ac.jp/~okumura/compression/lzss.c
 *
 * The main adaption have been done to move the code to a more
 * streaming approach to avoid storing data in the filesystem,
 * since many platforms using libpull does not have it.
 * The code is able to decompress data received from the network
 * calling multiple times the decode function
 */

#include <stdbool.h>
#include <stdint.h>
#include <libpull/pipeline/pipeline.h>

inline void pipeline_lzss_init(pipeline_ctx_t* ctx, void* more) {
    ctx->state = 0;
    ctx->finish = false;
    ctx->more = more;
}

int lzss_pipeline(pipeline_ctx_t* ctx, uint8_t* buf, int l);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBPULL_PIPELINE_LZSS_DECODER_H_ */
