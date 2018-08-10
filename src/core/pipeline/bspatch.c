/*-
 * Copyright 2003-2005 Colin Percival
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <libpull/pipeline/bspatch.h>

int pipeline_bspatch_init(pipeline_ctx_t* ctx, void* more) {
    ctx->finish = false;
    ctx->more = more;
    ctx->state = 0;
    return 0;
}

// Returns < 0 on error; >=0 number of bytes processed
int pipeline_bspatch_process(pipeline_ctx_t* ctx, uint8_t* buf, int len) {
    // TODO implement a check for the bundaries
    // A corrupted patch could let this program access random memory
    // The oldsize shuold be passed during the configuration, since if
    // it is embedded in the patch it could be used to access random bytes
	
	static int64_t oldpos,newpos;
	static ctrl_t ctrl[3];
	static int64_t i;
    static uint8_t j;
    static ctrl_t newsize;

    // bufp points to the number of bytes processed of the received buffer
    uint8_t* bufp = buf;
    uint8_t* old = (uint8_t*) ctx->stage_data;
    if (ctx->finish) {
        return 0;
    }

    printf("state %d, len %d\n", ctx->state, len);
    pipelineBegin
    {
	    oldpos=0;
        newpos=0;

        // Get header and check size
        for (j=0; j<24; j++) {
            while ((buf+len-bufp) == 0) {
                pipelineReturn(bufp-buf);
            }
            if (j < 16) {
                if (*bufp != 0x2a) {
                    printf("Invalid patch\n");
                    return -1;
                }
            } else {
                if (j != (16+8)) {
                    newsize.c[j-16] = *bufp;
                } else {
                    newsize.c[j-16]= *bufp & 0x7F;
                    newsize.y= ((*bufp & 0x80)? -newsize.y: newsize.y);
                }
            }
            bufp++;
        }
        if(newsize.y < 0) {
            return -2;
        }
    }
    printf("len is %d\n", newsize);
	while(newpos < newsize.y) {
        printf("len %d\n", len);
		/* Read control data */
		for(i=0;i<=2;i++) {
            for (j=0; j<8; j++) {
                while((buf+len-bufp) == 0) {
                    pipelineReturn(bufp-buf);
                }
                if (j != 7) {
                    ctrl[i].c[j] = *bufp;
                } else {
                    ctrl[i].c[j]= *bufp & 0x7F;
                    ctrl[i].y= ((*bufp & 0x80)? -ctrl[i].y: ctrl[i].y);
                }
                bufp++;
            }
		};

		/* Sanity-check */
		if(newpos+ctrl[0].y > newsize.y) {
            printf("exit 1 %d %d\n", newpos+ctrl[0].y , newsize.y);
			return -1;
        }

		/* Add old data to diff string */
        for(i=0;i<ctrl[0].y;i++) {
            while ((buf+len-bufp) == 0) {
                pipelineReturn(bufp-buf);
            }
            uint8_t buffer = *bufp + old[oldpos+i];
            ctx->next_func->process(ctx->next_ctx, &buffer, 1);
            bufp++;
        }

		/* Adjust pointers */
		newpos+=ctrl[0].y;
		oldpos+=ctrl[0].y;

		/* Sanity-check */
        if(newpos+ctrl[1].y > newsize.y) {
            printf("exit 2\n");
            return -1;
        }

		/* Read extra string */
        for (i=0; i<ctrl[1].y; i++) {
            while ((buf+len-bufp) == 0) {
                printf("hey\n");
                pipelineReturn(bufp-buf);
            }
            ctx->next_func->process(ctx->next_ctx, bufp, 1);
            bufp++;
        }

		/* Adjust pointers */
		newpos+=ctrl[1].y;
		oldpos+=ctrl[2].y;
	};
    ctx->finish = true;
    pipelineFinish
	return bufp-buf;
}

