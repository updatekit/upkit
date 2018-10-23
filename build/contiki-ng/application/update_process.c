#include "contiki.h"
#include "contiki-lib.h"
#include "dev/watchdog.h"

#include <libpull_agents/update.h>
#include <libpull/security.h>

#include "../../../default_keys.h"
#include "platform_headers.h"

PROCESS(update_process, "OTA Update process");

#define BUFFER_SIZE 512

void specialize_crypto_functions(void);
void specialize_conn_functions(void);

static digest_func df;
static ecc_func_t ef;
static void* conn_data;
static conn_type type;

static agent_event_t agent_event;
static update_agent_config cfg;
static update_agent_ctx_t ctx;
static uint8_t buffer[BUFFER_SIZE];
static struct etimer et;

static int8_t retries = 3;

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

void verify_before() {
#ifdef WITH_CRYPTOAUTHLIB
    ATCA_STATUS status = atcab_init(&cfg_ateccx08a_i2c_default);
    if (status != ATCA_SUCCESS) {
        log_error(GENERIC_ERROR, "Failure initializing ATECC508A\n");
    }
#endif
    WATCHDOG_STOP();
}

void verify_after() {
#ifdef WITH_CRYPTOAUTHLIB
    atcab_release();
#endif
    WATCHDOG_START();
}

PROCESS_THREAD(update_process, ev, data) {
    PROCESS_BEGIN();
    specialize_crypto_functions();
    specialize_conn_functions();
    conn_config(&cfg.subscriber, "coap://[fd00::1]", COAP_DEFAULT_PORT, PULL_UDP, NULL, "version");
    conn_config(&cfg.receiver, "coap://[fd00::1]", COAP_DEFAULT_PORT, PULL_UDP, NULL, "firmware");
    update_agent_reuse_connection(&cfg, 0);
    update_agent_set_identity(&cfg, identity_g);
    update_agent_vendor_keys(&cfg, vendor_x_g, vendor_y_g);
    update_agent_digest_func(&cfg, df);
    update_agent_ecc_func(&cfg, ef);
    update_agent_set_buffer(&cfg, buffer, BUFFER_SIZE);

    void* event_data;
    while (1) {
        agent_event = update_agent(&cfg, &ctx, &event_data);
        if (IS_FAILURE(agent_event)) {
            break;
        } else if (IS_CONTINUE(agent_event)) {
            if (agent_event == EVENT_APPLY) {
                watchdog_reboot();
                break;
            } else if (agent_event == EVENT_VERIFY_BEFORE) {
                verify_before();
            } else if (agent_event == EVENT_VERIFY_AFTER) {
                verify_after();
            } else if (agent_event == EVENT_CHECKING_UPDATES_TIMEOUT) {
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
