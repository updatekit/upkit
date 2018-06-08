#include <stdio.h>
#include <libpull/common.h>
#include <libpull/security.h>

#include <libpull_agents/update.h>

#include "default_keys.h"

#include "xtimer.h"
#include "fancy_leds.h"

#define TIMEOUT 1000
#define BUFFER_SIZE 1024

static agent_msg_t agent_msg;
static update_agent_config cfg;
static update_agent_ctx_t ctx;
static int8_t retries = 3;
static uint8_t success = 0;
static uint8_t buffer[BUFFER_SIZE];
static digest_func df;
static ecc_func_t ef;

void specialize_crypto_functions() {
#ifdef WITH_TINYDTLS
    df = tinydtls_digest_sha256;
    ef = tinydtls_secp256r1_ecc;
#elif WITH_TINYCRYPT
    df = tinycrypt_digest_sha256;
    ef = tinycrypt_secp256r1_ecc;
#endif
}

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

void* update_thread(void* args) {

    printf("Wait 5 seconds for the network to start\n");
    xtimer_sleep(5);

    specialize_crypto_functions();
    conn_config(&cfg.subscriber, "fd00:dead:beef::1", 5683, PULL_UDP, NULL, "/version");
    conn_config(&cfg.receiver, "fd00:dead:beef::1", 5683, PULL_UDP, NULL, "/firmware");
    update_agent_reuse_connection(&cfg, 0);
    update_agent_set_identity(&cfg, identity_g);
    update_agent_vendor_keys(&cfg, (uint8_t*) vendor_x_g, (uint8_t*) vendor_y_g);
    update_agent_digest_func(&cfg, df);
    update_agent_ecc_func(&cfg, ef);
    update_agent_set_buffer(&cfg, buffer, BUFFER_SIZE);

    while (1) {
        agent_msg = update_agent(&cfg, &ctx);
        if (IS_FAILURE(agent_msg)) {
            break;
        } else if (IS_CONTINUE(agent_msg)) {
            if (agent_msg.event == EVENT_APPLY) {
                success = 1;
                break;
            }
        } else if (IS_RECOVER(agent_msg)) {
            xtimer_sleep(5);
            if (retries-- > 0) {
                continue;
            } else {
                break;
            }
        } else if (IS_SEND(agent_msg)) {
            loop(GET_CONNECTION(agent_msg), TIMEOUT);
        }
    }
    printf("There was an error during the update phase\n");
    while(1);
}

static char update_thread_stack[THREAD_STACKSIZE_DEFAULT*8];
int main(void) {
    if (thread_create(update_thread_stack, sizeof(update_thread_stack), THREAD_PRIORITY_MAIN - 1,
                THREAD_CREATE_STACKTEST,
                update_thread, NULL, "update thread") <= KERNEL_PID_UNDEF) {
        puts("Error initializing thread\n");
    }

    while(1) {
        LED_FADE(LED0);
        xtimer_sleep(1);
    }
}

