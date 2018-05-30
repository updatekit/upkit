#include "contiki.h"
#include "contiki-lib.h"
#include "dev/watchdog.h"

#include <libpull_agents/update.h>
#include <libpull/security.h>

#include "default_configs.h"
#include "connection/connection_ercoap.h"
#include "platform_headers.h"

PROCESS(update_process, "OTA Update process");

#define BUFFER_SIZE 512

void specialize_crypto_functions(void);
void specialize_conn_functions(void);

static digest_func df;
static ecc_func_t ef;
static void* conn_data;
static conn_type type;

static agent_msg_t agent_msg;
static update_agent_config cfg;
static update_agent_ctx_t ctx;
static uint8_t buffer[BUFFER_SIZE];
static struct etimer et;

static int8_t retries = 3;

void verify_before() {
#ifdef WITH_CRYPTOAUTHLIB
    ATCA_STATUS status = atcab_init(&cfg_ateccx08a_i2c_default);
    if (status != ATCA_SUCCESS) {
        log_error(GENERIC_ERROR, "Failure initializing ATECC508A\n");
    }
#endif
    //XXX watchdog_stop();
}

void verify_after() {
#ifdef WITH_CRYPTOAUTHLIB
    atcab_release();
#endif
    //XXX watchdog_start(); disable until I make an interface for them
}

PROCESS_THREAD(update_process, ev, data) {
    PROCESS_BEGIN();
    specialize_crypto_functions();
    specialize_conn_functions();
    conn_config(&cfg.subscriber, "coap://[fd00::1]", COAP_DEFAULT_PORT, PULL_UDP, NULL, "version");
    conn_config(&cfg.receiver, "coap://[fd00::1]", COAP_DEFAULT_PORT, PULL_UDP, NULL, "firmware");
    update_agent_reuse_connection(&cfg, 0);
    update_agent_set_identity(&cfg, identity_g);
    update_agent_vendor_keys(&cfg, x, y);
    update_agent_digest_func(&cfg, df);
    update_agent_ecc_func(&cfg, ef);
    update_agent_set_buffer(&cfg, buffer, BUFFER_SIZE);

    while (1) {
        agent_msg = update_agent(&cfg, &ctx);
        if (IS_FAILURE(agent_msg)) {
             break;
         } else if (IS_CONTINUE(agent_msg)) {
             if (agent_msg.event == EVENT_APPLY) {
                 watchdog_reboot();
                 break;
             } else if (agent_msg.event == EVENT_VERIFY_BEFORE) {
                verify_before();
             } else if (agent_msg.event == EVENT_VERIFY_AFTER) {
                verify_after();
             } else if (agent_msg.event == EVENT_CHECKING_UPDATES_TIMEOUT) {
                 etimer_set(&et, (CLOCK_SECOND*5));
                 PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
             }
         } else if (IS_RECOVER(agent_msg)) {
             if (retries-- <= 0) {
                 continue;
             } else {
                 break;
             }
         } else if (IS_SEND(agent_msg)) {
             COAP_SEND(GET_CONNECTION(agent_msg));
         }
    } 
    if (retries <= 0) {
        log_info("The update process failed\n");
    }
    PROCESS_END();
}

void specialize_crypto_functions() {
#ifdef WITH_CRYPTOAUTHLIB
    df = cryptoauthlib_digest_sha256;
    ef = cryptoauthlib_ecc;
#elif WITH_TINYDTLS
    df = tinydtls_digest_sha256;
    ef = tinydtls_secp256r1_ecc;
#elif WITH_TINYCRYPT
    df = tinycrypt_digest_sha256;
    ef = tinycrypt_secp256r1_ecc;
#endif
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
