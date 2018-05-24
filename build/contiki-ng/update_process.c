#include "contiki.h"
#include "contiki-lib.h"
#include "dev/watchdog.h"
#include "agents/update.h"
#include "security/sha256.h"
#include "default_configs.h"
#include "transport/transport_ercoap.h"
#include "memory_headers.h"

PROCESS(update_process, "OTA Update process");

#define BUFFER_SIZE PAGE_SIZE

void specialize_crypto_functions(void);
void specialize_conn_functions(void);

static digest_func df;
static ecc_func_t ef;
static void* conn_data;
static conn_type type;

static agent_t agent;
static update_agent_config cfg;
static update_agent_ctx_t ctx;
static uint8_t buffer[BUFFER_SIZE];

static int8_t retries = 3;
static uint8_t success = 0;

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
        agent = update_agent(&cfg, &ctx);
        if (agent.current_error) {
            if (agent.required_action == FAILURE) {
                break;
            } else if (agent.required_action == RECOVER) {
                if (retries-- <= 0) {
                    continue;
                } else {
                    break;
                }
            }
        } else {
            if (agent.required_action == SEND) {
                if (agent.current_state == STATE_RECEIVE_SEND) {
                    COAP_SEND(ctx.rtxp);
                    continue;
                } else if (agent.current_state == STATE_CHECKING_UPDATES_SEND) {
                    COAP_SEND(ctx.stxp);
                    continue;
                }
            }
            if (agent.current_state == STATE_VERIFY_BEFORE) {
#ifdef WITH_CRYPTOAUTHLIB
                ATCA_STATUS status = atcab_init(&cfg_ateccx08a_i2c_default);
                if (status != ATCA_SUCCESS) {
                    log_error(GENERIC_ERROR, "Failure initializing ATECC508A\n");
                }
#endif
                watchdog_stop();
            } else if (agent.current_state == STATE_VERIFY_AFTER) {
                watchdog_start();
#ifdef WITH_CRYPTOAUTHLIB
                atcab_release();
#endif
            }
        }
        if (agent.current_state == STATE_APPLY) {
            success = 1;
            break;
        }
    }

    /*if (err) {
     * TODO find a way to implement this concept (maybe using messages?)
     log_info("Verification failed\n");
     err = invalidate_object(id, &obj_t);
     if (err) {
     log_error(err, "Error invalidating object");
     }
     continue;
     }
     */
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
