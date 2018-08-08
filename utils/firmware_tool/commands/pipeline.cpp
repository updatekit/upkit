#include <iostream>
#include <string.h>
#include "commands.hpp"

#include <libpull/memory.h>
#include <libpull/security.h>
#include <libpull/pipeline.h>

IMPLEMENT(pipeline)

int pipeline_compress_command(Context ctx) {
    std::cout << 1 << std::endl;
    return 0;
}

int pipeline_decompress_command(Context ctx) {
    std::cout << 2 << std::endl;
    return 0;
    
}

int pipeline_diff_command(Context ctx) {
    std::cout << 3 << std::endl;
    return 0;
    
}

int pipeline_patch_command(Context ctx) {
    std::cout << 4 << std::endl;
    return 0;
    
}
