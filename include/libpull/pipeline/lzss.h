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

#define EI 10  /* typically 10..13 */
#define EJ  4  /* typically 4..5 */
#define P   1  /* If match length <= P then output one character */
#define N (1 << EI)  /* buffer size */
#define F ((1 << EJ) + 1)  /* lookahead buffer size */

int pipeline_lzss_init(pipeline_ctx_t* ctx, void* more);
int pipeline_lzss_process(pipeline_ctx_t* ctx, uint8_t* buf, int l);
int pipeline_lzss_clear(pipeline_ctx_t*ctx);

static pipeline_func_t lzss_pipeline = {
    .init = pipeline_lzss_init,
    .process = pipeline_lzss_process,
    .clear = pipeline_lzss_clear
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBPULL_PIPELINE_LZSS_DECODER_H_ */
