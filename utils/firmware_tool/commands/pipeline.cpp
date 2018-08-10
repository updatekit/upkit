/* Copyright 2003-2005 Colin Percival
 * Copyright 2012 Matthew Endsley
 * Copyright 2018 Antonio Langiu
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <memory>

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

#define MIN(x,y) (((x)<(y)) ? (x) : (y))

extern "C" {

struct bsdiff_stream
{
 void* opaque;

 void* (*malloc)(size_t size);
 void (*free)(void* ptr);
 int (*write)(struct bsdiff_stream* stream, const void* buffer, int size);
};

static void split(int64_t *I,int64_t *V,int64_t start,int64_t len,int64_t h)
{
	int64_t i,j,k,x,tmp,jj,kk;

	if(len<16) {
		for(k=start;k<start+len;k+=j) {
			j=1;x=V[I[k]+h];
			for(i=1;k+i<start+len;i++) {
				if(V[I[k+i]+h]<x) {
					x=V[I[k+i]+h];
					j=0;
				};
				if(V[I[k+i]+h]==x) {
					tmp=I[k+j];I[k+j]=I[k+i];I[k+i]=tmp;
					j++;
				};
			};
			for(i=0;i<j;i++) V[I[k+i]]=k+j-1;
			if(j==1) I[k]=-1;
		};
		return;
	};

	x=V[I[start+len/2]+h];
	jj=0;kk=0;
	for(i=start;i<start+len;i++) {
		if(V[I[i]+h]<x) jj++;
		if(V[I[i]+h]==x) kk++;
	};
	jj+=start;kk+=jj;

	i=start;j=0;k=0;
	while(i<jj) {
		if(V[I[i]+h]<x) {
			i++;
		} else if(V[I[i]+h]==x) {
			tmp=I[i];I[i]=I[jj+j];I[jj+j]=tmp;
			j++;
		} else {
			tmp=I[i];I[i]=I[kk+k];I[kk+k]=tmp;
			k++;
		};
	};

	while(jj+j<kk) {
		if(V[I[jj+j]+h]==x) {
			j++;
		} else {
			tmp=I[jj+j];I[jj+j]=I[kk+k];I[kk+k]=tmp;
			k++;
		};
	};

	if(jj>start) split(I,V,start,jj-start,h);

	for(i=0;i<kk-jj;i++) V[I[jj+i]]=kk-1;
	if(jj==kk-1) I[jj]=-1;

	if(start+len>kk) split(I,V,kk,start+len-kk,h);
}

static void qsufsort(int64_t *I,int64_t *V,const uint8_t *old,int64_t oldsize)
{
	int64_t buckets[256];
	int64_t i,h,len;

	for(i=0;i<256;i++) buckets[i]=0;
	for(i=0;i<oldsize;i++) buckets[old[i]]++;
	for(i=1;i<256;i++) buckets[i]+=buckets[i-1];
	for(i=255;i>0;i--) buckets[i]=buckets[i-1];
	buckets[0]=0;

	for(i=0;i<oldsize;i++) I[++buckets[old[i]]]=i;
	I[0]=oldsize;
	for(i=0;i<oldsize;i++) V[i]=buckets[old[i]];
	V[oldsize]=0;
	for(i=1;i<256;i++) if(buckets[i]==buckets[i-1]+1) I[buckets[i]]=-1;
	I[0]=-1;

	for(h=1;I[0]!=-(oldsize+1);h+=h) {
		len=0;
		for(i=0;i<oldsize+1;) {
			if(I[i]<0) {
				len-=I[i];
				i-=I[i];
			} else {
				if(len) I[i-len]=-len;
				len=V[I[i]]+1-i;
				split(I,V,i,len,h);
				i+=len;
				len=0;
			};
		};
		if(len) I[i-len]=-len;
	};

	for(i=0;i<oldsize+1;i++) I[V[i]]=i;
}

static int64_t matchlen(const uint8_t *old,int64_t oldsize,const uint8_t *nnew,int64_t nnewsize)
{
	int64_t i;

	for(i=0;(i<oldsize)&&(i<nnewsize);i++)
		if(old[i]!=nnew[i]) break;

	return i;
}

static int64_t search(const int64_t *I,const uint8_t *old,int64_t oldsize,
		const uint8_t *nnew,int64_t nnewsize,int64_t st,int64_t en,int64_t *pos)
{
	int64_t x,y;

	if(en-st<2) {
		x=matchlen(old+I[st],oldsize-I[st],nnew,nnewsize);
		y=matchlen(old+I[en],oldsize-I[en],nnew,nnewsize);

		if(x>y) {
			*pos=I[st];
			return x;
		} else {
			*pos=I[en];
			return y;
		}
	};

	x=st+(en-st)/2;
	if(memcmp(old+I[x],nnew,MIN(oldsize-I[x],nnewsize))<0) {
		return search(I,old,oldsize,nnew,nnewsize,x,en,pos);
	} else {
		return search(I,old,oldsize,nnew,nnewsize,st,x,pos);
	};
}

static void offtout(int64_t x,uint8_t *buf)
{
	int64_t y;

	if(x<0) y=-x; else y=x;

	buf[0]=y%256;y-=buf[0];
	y=y/256;buf[1]=y%256;y-=buf[1];
	y=y/256;buf[2]=y%256;y-=buf[2];
	y=y/256;buf[3]=y%256;y-=buf[3];
	y=y/256;buf[4]=y%256;y-=buf[4];
	y=y/256;buf[5]=y%256;y-=buf[5];
	y=y/256;buf[6]=y%256;y-=buf[6];
	y=y/256;buf[7]=y%256;

	if(x<0) buf[7]|=0x80;
}

static int64_t writedata(struct bsdiff_stream* stream, const void* buffer, int64_t length)
{
	int64_t result = 0;

	while (length > 0)
	{
		const int smallsize = (int)MIN(length, INT_MAX);
		const int writeresult = stream->write(stream, buffer, smallsize);
		if (writeresult == -1)
		{
			return -1;
		}

		result += writeresult;
		length -= smallsize;
		buffer = (uint8_t*)buffer + smallsize;
	}

	return result;
}

struct bsdiff_request
{
	const uint8_t* old;
	int64_t oldsize;
	const uint8_t* nnew;
	int64_t nnewsize;
	struct bsdiff_stream* stream;
	int64_t *I;
	uint8_t *buffer;
};

static int bsdiff_internal(const struct bsdiff_request req)
{
	int64_t *I,*V;
	int64_t scan,pos,len;
	int64_t lastscan,lastpos,lastoffset;
	int64_t oldscore,scsc;
	int64_t s,Sf,lenf,Sb,lenb;
	int64_t overlap,Ss,lens;
	int64_t i;
	uint8_t *buffer;
	uint8_t buf[8 * 3];

	if((V=(int64_t*)req.stream->malloc((req.oldsize+1)*sizeof(int64_t)))==NULL) return -1;
	I = req.I;

	qsufsort(I,V,req.old,req.oldsize);
	req.stream->free(V);

	buffer = req.buffer;

	/* Compute the differences, writing ctrl as we go */
	scan=0;len=0;pos=0;
	lastscan=0;lastpos=0;lastoffset=0;
	while(scan<req.nnewsize) {
		oldscore=0;

		for(scsc=scan+=len;scan<req.nnewsize;scan++) {
			len=search(I,req.old,req.oldsize,req.nnew+scan,req.nnewsize-scan,
					0,req.oldsize,&pos);

			for(;scsc<scan+len;scsc++)
			if((scsc+lastoffset<req.oldsize) &&
				(req.old[scsc+lastoffset] == req.nnew[scsc]))
				oldscore++;

			if(((len==oldscore) && (len!=0)) || 
				(len>oldscore+8)) break;

			if((scan+lastoffset<req.oldsize) &&
				(req.old[scan+lastoffset] == req.nnew[scan]))
				oldscore--;
		};

		if((len!=oldscore) || (scan==req.nnewsize)) {
			s=0;Sf=0;lenf=0;
			for(i=0;(lastscan+i<scan)&&(lastpos+i<req.oldsize);) {
				if(req.old[lastpos+i]==req.nnew[lastscan+i]) s++;
				i++;
				if(s*2-i>Sf*2-lenf) { Sf=s; lenf=i; };
			};

			lenb=0;
			if(scan<req.nnewsize) {
				s=0;Sb=0;
				for(i=1;(scan>=lastscan+i)&&(pos>=i);i++) {
					if(req.old[pos-i]==req.nnew[scan-i]) s++;
					if(s*2-i>Sb*2-lenb) { Sb=s; lenb=i; };
				};
			};

			if(lastscan+lenf>scan-lenb) {
				overlap=(lastscan+lenf)-(scan-lenb);
				s=0;Ss=0;lens=0;
				for(i=0;i<overlap;i++) {
					if(req.nnew[lastscan+lenf-overlap+i]==
					   req.old[lastpos+lenf-overlap+i]) s++;
					if(req.nnew[scan-lenb+i]==
					   req.old[pos-lenb+i]) s--;
					if(s>Ss) { Ss=s; lens=i+1; };
				};

				lenf+=lens-overlap;
				lenb-=lens;
			};

			offtout(lenf,buf);
			offtout((scan-lenb)-(lastscan+lenf),buf+8);
			offtout((pos-lenb)-(lastpos+lenf),buf+16);

			/* Write control data */
			if (writedata(req.stream, buf, sizeof(buf)))
				return -1;

			/* Write diff data */
			for(i=0;i<lenf;i++)
				buffer[i]=req.nnew[lastscan+i]-req.old[lastpos+i];
			if (writedata(req.stream, buffer, lenf))
				return -1;

			/* Write extra data */
			for(i=0;i<(scan-lenb)-(lastscan+lenf);i++)
				buffer[i]=req.nnew[lastscan+lenf+i];
			if (writedata(req.stream, buffer, (scan-lenb)-(lastscan+lenf)))
				return -1;

			lastscan=scan-lenb;
			lastpos=pos-lenb;
			lastoffset=pos-scan;
		};
	};

	return 0;
}

int bsdiff(const uint8_t* old, int64_t oldsize, const uint8_t* nnew, int64_t nnewsize, struct bsdiff_stream* stream)
{
	int result;
	struct bsdiff_request req;

	if((req.I=(int64_t*)stream->malloc((oldsize+1)*sizeof(int64_t)))==NULL)
		return -1;

	if((req.buffer=(uint8_t*)stream->malloc(nnewsize+1))==NULL)
	{
		stream->free(req.I);
		return -1;
	}

	req.old = old;
	req.oldsize = oldsize;
	req.nnew = nnew;
	req.nnewsize = nnewsize;
	req.stream = stream;

	result = bsdiff_internal(req);

	stream->free(req.buffer);
	stream->free(req.I);

	return result;
}

} // End extern "C"

static int diff_write(struct bsdiff_stream* stream, const void* buffer, int size) {
    std::ofstream* output = (std::ofstream*)stream->opaque;
    output->write((const char*)buffer, size);
    return 0;
}

struct bspatch_header {
    char magic[16];
    char size[8];
};

/* This function should peform the diff between two files.
 * It requires the following values in input:
 * -b (binary_file): The first file used to make the diff
 * -z (second_binary_file): The second binary file used to make the diff
 * -f (out_file): The output file where the diff will be stored
 */
int pipeline_diff_command(Context ctx) {
    char* buffer1, *buffer2;
    int size1 = 0, size2 = 0;
    struct bsdiff_stream stream;
    // 0. Open the first input file
    std::ifstream input1(ctx.get_binary_file());
    if (! input1.is_open()) {
        std::cerr << "Error opening input file\n" << std::endl;
        return EXIT_FAILURE;
    }
    // 0. Open the second input file
    std::ifstream input2(ctx.get_second_binary_file());
    if (! input2.is_open()) {
        std::cerr << "Error opening input file\n" << std::endl;
        return EXIT_FAILURE;
    }
    // 2. Open the output file
    std::ofstream outfile(ctx.get_out_file());
    if (! outfile.is_open()) {
        std::cerr << "Error opening output file\n" << std::endl;
        return EXIT_FAILURE;
    }
    // 3. Read the files to a buffer
    input1.seekg(0, std::ios_base::end);
    size1 = input1.tellg();
    input1.seekg(0, std::ios_base::beg);
    buffer1 = new char [size2];
    input1.read(buffer1, size1);

    input2.seekg(0, std::ios_base::end);
    size2 = input2.tellg();
    input2.seekg(0, std::ios_base::beg);
    buffer2 = new char[size2];
    input2.read(buffer2, size2);

    // 4. Write the patch header
    bspatch_header header;
    for (int i=0; i<16; i++) {
        header.magic[i] = 0x2a;
    }
    offtout(size2, (uint8_t*) &header.size);
    outfile.write((char*)&header, sizeof(header));
 
    // 5. Start the algorithm
    stream.malloc = malloc;
    stream.free = free;
    stream.write = diff_write;
    stream.opaque = &outfile;
    return bsdiff((const unsigned char*) buffer1, size1, (const unsigned char*) buffer2, size2, &stream);
}

/*********** START BSPATCH ***********/

struct bspatch_stream
{
    void* opaque;
    int (*read)(const struct bspatch_stream* stream, void* buffer, int length);
};

int bspatch(const uint8_t* old, int64_t oldsize, uint8_t* nnew, int64_t nnewsize, struct bspatch_stream* stream);

static int64_t offtin(uint8_t *buf)
{
    int64_t y;

    y=buf[7]&0x7F;
    y=y*256;y+=buf[6];
    y=y*256;y+=buf[5];
    y=y*256;y+=buf[4];
    y=y*256;y+=buf[3];
    y=y*256;y+=buf[2];
    y=y*256;y+=buf[1];
    y=y*256;y+=buf[0];

    if(buf[7]&0x80) y=-y;

    return y;
}

int bspatch(const uint8_t* old, int64_t oldsize, uint8_t* nnew, int64_t nnewsize, struct bspatch_stream* stream)
{
    uint8_t buf[8];
    int64_t oldpos,nnewpos;
    int64_t ctrl[3];
    int64_t i;

    oldpos=0;nnewpos=0;
    while(nnewpos<nnewsize) {
        /* Read control data */
        for(i=0;i<=2;i++) {
            if (stream->read(stream, buf, 8))
                return -1;
            ctrl[i]=offtin(buf);
        };

        /* Sanity-check */
        if(nnewpos+ctrl[0]>nnewsize)
            return -1;

        /* Read diff string */
        if (stream->read(stream, nnew + nnewpos, ctrl[0]))
            return -1;

        /* Add old data to diff string */
        for(i=0;i<ctrl[0];i++)
            if((oldpos+i>=0) && (oldpos+i<oldsize))
                nnew[nnewpos+i]+=old[oldpos+i];

        /* Adjust pointers */
        nnewpos+=ctrl[0];
        oldpos+=ctrl[0];

        /* Sanity-check */
        if(nnewpos+ctrl[1]>nnewsize)
            return -1;

        /* Read extra string */
        if (stream->read(stream, nnew + nnewpos, ctrl[1]))
            return -1;

        /* Adjust pointers */
        nnewpos+=ctrl[1];
        oldpos+=ctrl[2];
    };

    return 0;
}

static int patch_read(const struct bspatch_stream* stream, void* buffer, int length) {
    std::ifstream* input;
    static int64_t val = 0;
    val += length;
    input = (std::ifstream*) stream->opaque;
    if (input->eof()) {
        return -1;
    }
    input->read((char*)buffer, length);
    return 0;
}

/* This function applies the patch passed in input to generate the original file.
 * This function needs the following arguments from the context:
 * -b (binary_file): file where to apply the patch
 * -z (second_binary_file): patch
 * -f (output_file): patched file
 */
int pipeline_patch_command(Context ctx) {
    char* buffer1;
    int size1 = 0, size2 = 0;
    struct bspatch_stream stream;
    // 0. Open the first input file
    std::ifstream input1(ctx.get_binary_file());
    if (! input1.is_open()) {
        std::cerr << "Error opening input file\n" << std::endl;
        return EXIT_FAILURE;
    }
    // 1. Open the second input file
    std::ifstream input2(ctx.get_second_binary_file());
    if (! input2.is_open()) {
        std::cerr << "Error opening input file\n" << std::endl;
        return EXIT_FAILURE;
    }
    // 2. Open the output file
    std::ofstream outfile(ctx.get_out_file());
    if (! outfile.is_open()) {
        std::cerr << "Error opening output file\n" << std::endl;
        return EXIT_FAILURE;
    }
    // 3. Read the files to a buffer
    input1.seekg(0, std::ios_base::end);
    size1 = input1.tellg();
    input1.seekg(0, std::ios_base::beg);
    buffer1 = new char [size2];
    input1.read(buffer1, size1);

    // 4. Prepare for the patching process
    stream.read = patch_read;
    stream.opaque = &input2;

    // 4.1 Get the nnew file size and alloc a buffer with that size
    struct bspatch_header header;
    input2.read((char*)&header, sizeof(header));
    if (input2.eof()) {
        std::cerr << "Invalid patch: patch too small" << std::endl;
        return -1;
    }

    int64_t size = offtin((uint8_t*)header.size);
    if (size < 0) {
        std::cerr << "Invalid patch: invalid size" << std::endl;
        return -1;
    }
    char* buffer = new char [size];
    
    // 5. Start the algorithm
    int ret = bspatch((const unsigned char*) buffer1, size1, (unsigned char*) buffer, size, &stream);

    if (ret != 0) {
        std::cerr << "Error during the patching process" << std::endl;
        input1.close();
        input2.close();
        outfile.close();
        return -1;
    }
    // 6. Store the patched file
    outfile.write(buffer, size);
    
    // 7. Close all files and clean buffer
    input1.close();
    input2.close();
    outfile.close();
    return 0;
}
