#include <stdio.h>
#include <libpull/common.h>
#include <libpull/security.h>

#include <libpull_agents/update.h>

#include "default_keys.h"

#define TIMEOUT 1000
#define BUFFER_SIZE 1024

#define SERVER_ADDR "fd11:22::c86e:7efd:33a9:a69e"

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

static agent_msg_t agent_msg;
static update_agent_config cfg;
static update_agent_ctx_t ctx;
static int8_t retries = 3;
static uint8_t success = 0;
static uint8_t buffer[BUFFER_SIZE];

int main(void) {
    conn_config_t conn;
    conn_config(&conn, SERVER_ADDR, 5683, PULL_UDP, NULL);

    update_agent_config(&cfg, &conn, safestore_g, buffer, BUFFER_SIZE);

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
    while(1) { /* BUSY WAIT */};
}
