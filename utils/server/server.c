#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <coap/coap.h>
#include <coap/coap_dtls.h>

#include "resources.h"
#include "config.h"
#include "server.h"

#include "memory/manifest.h"

static int server_exit = 0;
static server_ctx_t server_ctx = {
    .addr_str = "::",
    .log_level = LOG_WARNING,
    .assets_dir = ".",
    .mapped_file = NULL,
    .mapped_file_len = 0
};

void update_server_context(server_ctx_t* ctx) {
    int fwd = -1;
    char firmware[MAX_PATH_LEN];
    firmware[0] = '\0';
    strncat(firmware, ctx->assets_dir, MAX_PATH_LEN);
    strncat(firmware, "/", MAX_PATH_LEN);
    strncat(firmware, DEFAULT_FIRMWARE_NAME, MAX_PATH_LEN);
    if ((fwd = open(firmware, O_RDONLY)) < 0) {
        fprintf(stderr, "Impossible to open the firmware file\n");
        exit(EXIT_FAILURE);
    };
    if (read(fwd, &ctx->mt, sizeof(manifest_t)) != sizeof(manifest_t)) {
        fprintf(stderr, "Error reading firmware manifest\n");
        exit(EXIT_FAILURE);
    }
    if (ctx->mapped_file) {
        munmap((void*) ctx->mapped_file, ctx->mapped_file_len);
        ctx->mapped_file = NULL;
        ctx->mapped_file_len = 0;
    }
    /* Get the size of the file. */
    struct stat s;
    if (fstat (fwd, &s) < 0) {
        fprintf(stderr, "Impossible to get the size of the file\n");
        exit(EXIT_FAILURE);
    }
    ctx->mapped_file_len = s.st_size;
    /* Memory-map the file. */
    ctx->mapped_file = mmap (0, ctx->mapped_file_len, PROT_READ, MAP_PRIVATE, fwd, 0);
    if (ctx->mapped_file == MAP_FAILED) {
        fprintf(stderr, "Failed to map the file %s\n", strerror (errno));
        exit(EXIT_FAILURE);
    }
    close(fwd);
}

static void handle_sigint(int signum) {
    server_exit = 1;
}

void usage(const char*);
coap_context_t* get_coap_context(const char *node);

int main(int argc, char** argv) {
    coap_context_t* coap_ctx = NULL;

    int opt;
    // (0) Parse parameters
    while ((opt = getopt(argc, argv, "A:v:l:a:")) != -1) {
     switch (opt) {
     case 'A':
       strncpy(server_ctx.addr_str, optarg, MAX_ADDR_LEN-1);
       server_ctx.addr_str[MAX_ADDR_LEN-1] = '\0';
       break;
    case 'a':
       strncpy(server_ctx.assets_dir, optarg, MAX_PATH_LEN-1);
       server_ctx.assets_dir[MAX_PATH_LEN-1] = '\0';
       break;
     case 'v':
       server_ctx.log_level = strtol(optarg, NULL, 10);
       break;
     case 'l':
       if (!coap_debug_set_packet_loss(optarg)) {
           usage(argv[0]);
           return EXIT_FAILURE;
       }
       break;
     default:
       usage(argv[0]);
       return EXIT_FAILURE;
     }
    }
    // (1) Init libcoap
    coap_startup();
    coap_dtls_set_log_level(server_ctx.log_level);
    coap_set_log_level(server_ctx.log_level);
    // (2) Create the libcoap context
    coap_ctx = get_coap_context(server_ctx.addr_str);
    if (!coap_ctx) {
        printf("Error creating context\n");
        return EXIT_FAILURE;
    }
    // (3) Catch signal
    signal(SIGINT, handle_sigint);
    // (4) Update Server Context
    update_server_context(&server_ctx);
    coap_set_app_data(coap_ctx, &server_ctx);
    // (5) Register resources
    if (!init_resources(&server_ctx, coap_ctx)) {
        fprintf(stderr, "Error setting resources\n");
        return EXIT_FAILURE;
    }
    // (6) Start serving
    while (!server_exit) {
        int result = coap_run_once(coap_ctx, 2000);
        if (result < 0) {
            break;
        }
        coap_check_notify(coap_ctx);
    }
    // (7) Free context
    coap_free_context(coap_ctx);
    coap_cleanup();
    return 0;
}

void usage(const char* progname) {
    printf("usage: %s [-A host] [-v verbosity] [-l packet_loss]\n", progname);
    printf("The server has been built with the following parameters:\n");
    printf("\tCOAP_DEFAULT_PORT:\t%d", COAP_DEFAULT_PORT);
    printf("\tCOAPS_DEFAULT_PORT:\t%d", COAPS_DEFAULT_PORT);
}
