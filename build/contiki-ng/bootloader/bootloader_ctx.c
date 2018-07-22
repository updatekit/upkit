#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>

#include <libpull_agents/bootloader_ctx.h>


int main(int argc, char**argv) {
    if (argc != 2) {
        printf("usage: %s file_name\n", argv[0]);
        return EXIT_FAILURE;
    }
    int fd = -1;
    bootloader_ctx ctx;
    bzero(&ctx, sizeof(ctx));
    ctx.startup_flags = FIRST_BOOT;
    fd = open(argv[1], O_WRONLY|O_CREAT|O_TRUNC, 0777);
    if (fd < 0) {
        printf ("Error: impossible to write the file %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (write(fd, &ctx, sizeof(ctx)) != sizeof(ctx)) {
        printf ("Error: file not correcly written\n");
        return EXIT_FAILURE;
    }     
    close(fd);
    return EXIT_SUCCESS;
}
