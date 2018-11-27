#include <stdio.h>
#include <libpull/common.h>
#include <libpull/security.h>

#include <libpull_agents/update.h>

#include "default_keys.h"

#define TIMEOUT 1000

#define SERVER_ADDR "fd11:22::c86e:7efd:33a9:a69e"

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

static agent_event_t agent_event;
static update_agent_config_t cfg;
static update_agent_ctx_t ctx;
static bool exitb = false;

int main(void) {
    conn_config_t conn;
    agent_data_t event_data;
    conn_config(&conn, SERVER_ADDR, 5683, PULL_UDP, NULL);

    update_agent_config(&cfg, &conn, &safestore_g);

    do {
        agent_event = update_agent(&cfg, &ctx, &event_data);
        if (IS_FAILURE(agent_event)) {
            log_debug("Failure\n");
            exitb = true;
            break;
        } else if (IS_SEND(agent_event)) {
            loop(GET_CONNECTION(agent_event), TIMEOUT);
        } else if (IS_CONTINUE(agent_event)) {
            if (agent_event == EVENT_APPLY) {
                exitb = true;
                break;
            } else {
                log_debug("Continue..\n");
            }
        }
    } while (!exitb);
    printf("There was an error during the update phase\n");
    while(1) { /* BUSY WAIT */};
}
