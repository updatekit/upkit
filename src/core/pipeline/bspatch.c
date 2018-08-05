#include <libpull/pipeline.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

int pipeline_bspatch_init(pipeline_ctx_t* ctx, pipeline_write write, void* more) {
    ctx->write = write;
    ctx->more = more;
    ctx->state = 0;
    return 0;
}

// Returns < 0 on error; >=0 number of bytes processed
int pipeline_bspatch_process(pipeline_ctx_t* ctx, uint8_t* buf, int len) {
    // TODO implement a check for the boundaries
    // A corrupted patch could let this program access random memory
    // The oldsize shuold be passed during the configuration, since if
    // it is embedded in the patch it could be used to access random bytes
    static ctrl_t ctrl;
    static uint8_t i;
    static bspatch_state_t bspatch_state;
    uint8_t buffer; 

    // bufp points to the number of bytes processed of the received buffer
    uint8_t* bufp = buf;
    uint8_t* oldp = (uint8_t*) ctx->stage_data;

    pipelineBegin
        i = 0;
    ctrl.y = 24; // header size
    bspatch_state = READ_HEADER;
    while(1) {
        while((buf+len-bufp) == 0) {
            pipelineReturn(0);
        }
        switch(bspatch_state) {
            case READ_HEADER:
                // read header
                if (i < 16) {
                    if (*bufp != 0x2a) {
                        return -1;
                    }
                }
                i++;
                bufp++;
                break;
            case READ_CTRL:
                if (i != 4) {
                    ctrl.c[i] = *bufp;
                } else {
                    ctrl.c[i]= *bufp & 0x7F;
                    ctrl.y= ((*bufp & 0x80)? -ctrl.y: ctrl.y);
                    bspatch_state++;
                    i=0;
                }
                bufp++;
                i++;
                continue;
            case SUM:
                buffer = *bufp + *oldp;
                ctx->write(&buffer, 1, ctx);
                bufp++;
                oldp++;
                break;
            case INSERT:
                ctx->write(bufp, 1, ctx);
                bufp++;
                break;
            case SHIFT:
                oldp+=ctrl.y;
                ctrl.y = 0;
                break;
        }
        if (ctrl.y-- <= 0) {
            bspatch_state = (bspatch_state%3)+1;
        }
    };
    pipelineFinish
    // This function always returns inside the while
}

