#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <libpull/memory.h>
#include <libpull/security.h>
#include <libpull/pipeline.h>

#include "commands.hpp"

IMPLEMENT(pipeline)

/*****************************************************************************/
/********************** Start LZSS encoder and decoder ***********************/
/*****************************************************************************/

/* The LZSS encoder and decoder and inspired from the
 * Haruhiko Okumura version released to the public domain */
#define EI 10  /* typically 10..13 */
#define EJ  4  /* typically 4..5 */
#define P   1  /* If match length <= P then output one character */
#define N (1 << EI)  /* buffer size */
#define F ((1 << EJ) + 1)  /* lookahead buffer size */
int bit_buffer = 0, bit_mask = 128;
unsigned long codecount = 0, textcount = 0;

void putbit1(std::ofstream &outfile) {
    bit_buffer |= bit_mask;
    if ((bit_mask >>= 1) == 0) {
        outfile.put(bit_buffer);
        bit_buffer = 0;  bit_mask = 128;  codecount++;
    }
}

void putbit0(std::ofstream &outfile)
{
    if ((bit_mask >>= 1) == 0) {
        outfile.put(bit_buffer);
        bit_buffer = 0;  bit_mask = 128;  codecount++;
    }
}

void flush_bit_buffer(std::ofstream &outfile)
{
    if (bit_mask != 128) {
        outfile.put(bit_buffer);
        codecount++;
    }
}

void output1(std::ofstream &outfile, int c)
{
    int mask;

    putbit1(outfile);
    mask = 256;
    while (mask >>= 1) {
        if (c & mask) putbit1(outfile);
        else putbit0(outfile);
    }
}

void output2(std::ofstream &outfile, int x, int y)
{
    int mask;

    putbit0(outfile);
    mask = N;
    while (mask >>= 1) {
        if (x & mask) putbit1(outfile);
        else putbit0(outfile);
    }
    mask = (1 << EJ);
    while (mask >>= 1) {
        if (y & mask) putbit1(outfile);
        else putbit0(outfile);
    }
}

/* The compress functions needs to get the
 * following values from the context:
 * -b (binary_file): the input file
 * -f (output_file): the file where to store the compressed version
 */
int pipeline_compress_command(Context ctx) {
    int i, j, f1, x, y, r, s, bufferend;
    static unsigned char buffer[N * 2];
    char c;

    // 0. Open the input file
    std::ifstream input(ctx.get_binary_file());
    if (! input.is_open()) {
        std::cerr << "Error opening input file\n" << std::endl;
        return EXIT_FAILURE;
    }
    // 1. Open the output file
    std::ofstream outfile(ctx.get_out_file());
    if (! outfile.is_open()) {
        std::cerr << "Error opening output file\n" << std::endl;
        return EXIT_FAILURE;
    }

    // 2. Start the algorithm
    for (i = 0; i < N - F; i++) buffer[i] = ' ';
    for (i = N - F; i < N * 2; i++) {
        input.get(c);
        if (input.eof()) {
            break;
        }

        buffer[i] = c;  textcount++;
    }
    bufferend = i;  r = N - F;  s = 0;
    while (r < bufferend) {
        f1 = (F <= bufferend - r) ? F : bufferend - r;
        x = 0;  y = 1;  c = buffer[r];
        for (i = r - 1; i >= s; i--)
            if (buffer[i] == c) {
                for (j = 1; j < f1; j++)
                    if (buffer[i + j] != buffer[r + j]) break;
                if (j > y) {
                    x = i;  y = j;
                }
            }
        if (y <= P) {  y = 1;  output1(outfile, c);  }
        else output2(outfile, x & (N - 1), y - 2);
        r += y;  s += y;
        if (r >= N * 2 - F) {
            for (i = 0; i < N; i++) buffer[i] = buffer[i + N];
            bufferend -= N;  r -= N;  s -= N;
            while (bufferend < N * 2) {
                input.get(c);
                if (input.eof()) {
                    break;
                }
                buffer[bufferend++] = c;  textcount++;
            }
        }
    }
    flush_bit_buffer(outfile);
    printf("text:  %ld bytes\n", textcount);
    printf("code:  %ld bytes (%ld%%)\n",
            codecount, (codecount * 100) / textcount);
    return 0;
}

static int getbit(std::ifstream &input, int n) {
    int i, x;
    static int buf, mask = 0;
    char c;
    x = 0;
    for (i = 0; i < n; i++) {
        if (mask == 0) {
            input.get(c);
            if (input.eof()) {
                return EOF;
            }
            buf = c;
            mask = 128;
        }
        x <<= 1;
        if (buf & mask) x++;
        mask >>= 1;
    }
    return x;
}

/* This function receives a compressed file as input and
 * decompresses it to another file. 
 * The function needs to get the following parameters from
 * the context:
 * -b (binary_file): the compressed input file
 * -f (output_file): the file where to store the decompressed version
 */
int pipeline_decompress_command(Context ctx) {
    // ctx->binary_file is the input file name
    // ctx->out_file is the output file name
    int i, j, k, r, c;
    static unsigned char buffer[N * 2];

    // 0. Open the input file
    std::ifstream input(ctx.get_binary_file());
    if (! input.is_open()) {
        std::cerr << "Error opening input file\n" << std::endl;
        return EXIT_FAILURE;
    }
    // 1. Open the output file
    std::ofstream output(ctx.get_out_file());
    if (! output.is_open()) {
        std::cerr << "Error opening output file\n" << std::endl;
        return EXIT_FAILURE;
    }

    // 2. Start the algorithm
    for (i = 0; i < N - F; i++) buffer[i] = ' ';
    r = N - F;
    while ((c = getbit(input, 1)) != EOF) {
        if (c) {
            if ((c = getbit(input, 8)) == EOF) break;
            output.put(c);
            buffer[r++] = c;  r &= (N - 1);
        } else {
            if ((i = getbit(input, EI)) == EOF) break;
            if ((j = getbit(input, EJ)) == EOF) break;
            for (k = 0; k <= j + 1; k++) {
                c = buffer[(i + k) & (N - 1)];
                output.put(c);
                buffer[r++] = c;  r &= (N - 1);
            }
        }
    }
    return 0;
}

/*****************************************************************************/
/************************ Start bsdiff and bspatch ***************************/
/*****************************************************************************/

int pipeline_diff_command(Context ctx) {
    return 0;
}

int pipeline_patch_command(Context ctx) {
    return 0;
}
