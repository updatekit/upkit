#include <stdio.h>
#include <libpull/common.h>
#include <libpull/security.h>

#include <libpull_agents/update.h>

#include "default_keys.h"

#include "xtimer.h"
#include "fancy_leds.h"

#define TIMEOUT 1000
#define BUFFER_SIZE 1024
#define SERVER_ADDR "fd11:22::c86e:7efd:33a9:a69e"

static agent_event_t agent_event;
static update_agent_config_t cfg;
static update_agent_ctx_t ctx;
static uint8_t buffer[BUFFER_SIZE];
static bool loop_exit = false;

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

void* update_thread(void* args) {
    printf("Wait 5 seconds for the network to start\n");
    xtimer_sleep(5);

    conn_config_t conn;
    agent_data_t event_data;
    conn_config(&conn, SERVER_ADDR, 5683, PULL_UDP, NULL);

    update_agent_config(&cfg, &conn, &safestore_g, buffer, BUFFER_SIZE);

    do {
        agent_event = update_agent(&cfg, &ctx, &event_data);
        if (IS_FAILURE(agent_event)) {
            log_debug("Failure\n");
            loop_exit = true;
            break;
        } else if (IS_SEND(agent_event)) {
            loop(GET_CONNECTION(agent_event), TIMEOUT);
        } else if (IS_CONTINUE(agent_event)) {
            if (agent_event == EVENT_APPLY) {
                loop_exit = true;
                break;
            } else {
                log_debug("Continue..\n");
            }
        }
    } while (!loop_exit);
    printf("There was an error during the update phase\n");
    while(1) { /* BUSY WAIT */};
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

