#include "contiki.h"
#include "contiki-lib.h"
#include "dev/watchdog.h"

#include <libpull_agents/update.h>
#include <libpull/security.h>

#include "../../../default_keys.h"
#include "platform_headers.h"

PROCESS(update_process, "OTA Update process");

void specialize_conn_functions(void);
static void* conn_data;
static conn_type type;

static agent_event_t agent_event;
static update_agent_config_t cfg;
static update_agent_ctx_t ctx;
static struct etimer et;

static int8_t retries = 3;

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

PROCESS_THREAD(update_process, ev, data) {
    PROCESS_BEGIN();
    agent_data_t event_data;

    specialize_conn_functions();

     /* Configure connection */
     conn_config_t conn;
     conn_config(&conn, "coap://[fd00::1]", 0, type, conn_data);

     /* Configure update agent */
     update_agent_config(&cfg, &conn, &safestore_g);

     while (1) {
         agent_event = update_agent(&cfg, &ctx, &event_data);
         if (IS_FAILURE(agent_event)) {
             break;
         } else if (IS_CONTINUE(agent_event)) {
             if (agent_event == EVENT_APPLY) {
                 watchdog_reboot();
                 break;
             } else if (agent_event == EVENT_VERIFY_BEFORE) {
                #ifdef WITH_CRYPTOAUTHLIB
                 ATCA_STATUS status = atcab_init(&cfg_ateccx08a_i2c_default);
                 if (status != ATCA_SUCCESS) {
                     log_error(GENERIC_ERROR, "Failure initializing ATECC508A\n");
                 }
                #endif
                 WATCHDOG_STOP();
             } else if (agent_event == EVENT_VERIFY_AFTER) {
                #ifdef WITH_CRYPTOAUTHLIB
                atcab_release();
                #endif
                WATCHDOG_START();
             } else if (agent_event == EVENT_SUBSCRIBE_TIMEOUT) {
                 etimer_set(&et, (CLOCK_SECOND*5));
                 PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
             }
         } else if (IS_RECOVER(agent_event)) {
             if (retries-- <= 0) {
                 continue;
             } else {
                 break;
             }
         } else if (IS_SEND(agent_event)) {
             COAP_SEND(GET_CONNECTION(event_data));
         }
     } 
     if (retries <= 0) {
         log_info("The update process failed\n");
     }
     PROCESS_END();
}

void specialize_conn_functions() {
#ifdef CONN_UDP
    type = PULL_UDP;
    conn_data = NULL;
#elif CONN_DTLS_PSK
    type = PULL_DTLS_PSK;
    conn_data = NULL;
#endif
}
