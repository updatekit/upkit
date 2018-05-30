#include "commands.hpp"
#include <libpull_agents/bootloader_ctx.h>
#include <iostream>
#include <fstream>

IMPLEMENT(bootctx)

int bootctx_generate_command(Context ctx) {
    bootloader_ctx bctx = {};
    bctx.startup_flags = FIRST_BOOT;
    std::ofstream out(ctx.get_out_file(),  std::ios_base::binary | std::ios_base::out);
    if (!out) {
        std::cout << "Impossible to create the bootloader ctx" << std::endl;
        return EXIT_FAILURE;
    }
    out.write((char*)  &bctx, sizeof(bctx));
    out.close();
    return EXIT_SUCCESS;
}
