#include "contiki.h"
#include "contiki-lib.h"
#include "dev/watchdog.h"
#include "button-sensor.h"
#include "leds.h"

#include "agents/update.h"
#include "security/sha256.h"

#include "default_configs.h"
#include "transport/transport_ercoap.h"
#include "memory_headers.h"

#define HARDCODED_PROV_SERVER(addr) uip_ip6addr(addr,0xfd00,0x0,0x0,0x0,0x0,0x0,0x0,0x1)

#if TARGET == srf06-cc26xx
#include "driverlib/flash.h"
#endif

#define BUFFER_LEN PAGE_SIZE

#ifdef WITH_CRYPTOAUTHLIB
DIGEST_FUNC(cryptoauthlib);
#elif WITH_TINYDTLS
DIGEST_FUNC(tinydtls);
#elif WITH_TINYCRYPT
DIGEST_FUNC(tinycrypt);
// To perform the verification I do not need any rng. Define e dummy
// function to make tinycrypt happy.
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

PROCESS(update_process, "OTA Update process");
PROCESS(main_process, "Main process");
AUTOSTART_PROCESSES(&main_process);


static struct etimer et;
static digest_func df;
static ecc_func_t ef;
static void* conn_data;
static conn_type type;

static identity_t identity_g = {
    .udid = 0x1234,
    .random = 0x5678
};

#include "net/ipv6/uip-icmp6.h"
static uip_ipaddr_t server_addr;
static struct uip_icmp6_echo_reply_notification echo_reply_notification;
static uint8_t echo_reply = 0;

static void echo_reply_handler(uip_ipaddr_t *source, uint8_t ttl, uint8_t *data, uint16_t datalen) {
    echo_reply++;
}


void test_memory() {
    /* This is a test to ensure that the image can not write the internal
     * flash memory */
#ifdef DEBUG
    log_info("Trying to write internal memory");
    uint8_t buff[32];
    uint8_t i = 0xff;
    memset(buff, 0x0, 32);
    while (i>0) {
        if (FlashProgram((uint8_t*) &buff, 0x0, 32) == FAPI_STATUS_SUCCESS) {
            break;
        }
        i--;
    }
    log_info("\nMemory is %s\n", i==0? "blocked": "not blocked");
#endif
}

static struct etimer et_led;
PROCESS_THREAD(main_process, ev, data) {
    PROCESS_BEGIN();
    etimer_set(&et, (CLOCK_SECOND*5));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    uip_icmp6_echo_reply_callback_add(&echo_reply_notification, echo_reply_handler);
    HARDCODED_PROV_SERVER(&server_addr);
    do {
        uip_icmp6_send(&server_addr, ICMP6_ECHO_REQUEST, 0, UIP_ICMP6_ECHO_REQUEST_LEN);
        etimer_set(&et, (CLOCK_SECOND/2 * (echo_reply == 0? 10:1)));
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    } while(!echo_reply);
    process_start(&update_process, NULL);

    do {
        leds_toggle(LEDS_RED);
        etimer_set(&et_led, (CLOCK_SECOND*1));
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et_led));
    } while (1);
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

#define BUFFER_SIZE 1024

static agent_t agent;
static update_agent_config cfg;
static update_agent_ctx_t ctx;
static int8_t retries = 3;
static uint8_t success = 0;
static uint8_t buffer[BUFFER_SIZE];

PROCESS_THREAD(update_process, ev, data) {
    PROCESS_BEGIN();
    specialize_crypto_functions();
    specialize_conn_functions();
    conn_config(&cfg.subscriber, "coap://[fd00::1]", COAP_DEFAULT_PORT, PULL_UDP, NULL, "version");
    conn_config(&cfg.receiver, "coap://[fd00::1]", 0, COAP_DEFAULT_PORT, NULL, "firmware");
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
