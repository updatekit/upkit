#include "contiki.h"
#include "contiki-lib.h"
#include "dev/watchdog.h"
#include "leds.h"

#include "common/libpull.h"
#include "memory/memory_objects.h"
#include "memory/manifest.h"
#include "network/receiver.h"
#include "network/subscriber.h"
#include "security/verifier.h"
#include "security/sha256.h"

#include "default_configs.h"
#include "transport/transport_ercoap.h"
#include "memory_headers.h"

#if TARGET == srf06-cc26xx
#include "driverlib/flash.h"
#endif

#define BUFFER_LEN PAGE_SIZE

#ifdef WITH_CRYPTOAUTHLIB
#include <cryptoauthlib.h>
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
AUTOSTART_PROCESSES(&update_process, &main_process);

static txp_ctx txp;
static subscriber_ctx sctx;
static receiver_ctx rctx;

static obj_id id;
static mem_object obj_t;
static mem_object new_firmware;
static struct etimer et;

static void* conn_data;
static conn_type type;

static digest_func df;
static ecc_func_t ef;

static identity_t identity_g = {
    .udid = 0x1234,
    .random = 0x5678
};

static struct etimer et_led;
PROCESS_THREAD(main_process, ev, data) {
    PROCESS_BEGIN();
#ifdef DEBUG
    /* This is a test to ensure that the image can not write the internal
     * flash memory */
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
    do {
        leds_toggle(LEDS_RED);
        etimer_set(&et_led, (CLOCK_SECOND*1));
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et_led));
    } while (1);
    PROCESS_END();
}

void specialize_crypto_functions() {
#if WITH_CRYPTOAUTHLIB
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
    type = TXP_UDP;
    conn_data = NULL;
#elif CONN_DTLS_ECDSA
    type = TXP_DTLS_ECDSA;
    conn_data = &dtls_ecdsa_data;
#elif CONN_DTLS_PSK
    type = TXP_DTLS_PSK;
    conn_data = &dtls_psk_data;
#endif
}

PROCESS_THREAD(update_process, ev, data) {
    PROCESS_BEGIN();
    specialize_crypto_functions();
    specialize_conn_functions();

    txp_init(&txp, "", COAPS_DEFAULT_PORT, type, conn_data);
    log_info("Subscribing to the server\n");
    pull_error err = subscribe(&sctx, &txp, "/version", &obj_t);
    if (err) {
        log_error(err,"Error subscribing to the provisioning server\n");
        return EXIT_FAILURE;
    }
    while(1) {
        // Check if there are updates
        log_info("Checking for updates\n");
        while (!sctx.has_updates) {
            check_updates(&sctx, subscriber_cb); // check for errors
            COAP_SEND(txp);
            etimer_set(&et, (CLOCK_SECOND*3));
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        }
        // get the oldest slot
        uint16_t version;
        err = get_oldest_firmware(&id, &version, &obj_t);
        if (err) {
            log_error(err, "Error getting the oldest slot\n");
            continue;
        }
        // open the oldest slot
        err = memory_open(&new_firmware, id, WRITE_ALL);
        if (err) {
            log_error(err, "Error opening the object for storing\n");
            continue;
        }
        // download the image to the oldest slot
        // XXX refactor the code to close the opened firmware at each failure
        err = receiver_open(&rctx, &txp, identity_g, "firmware", &new_firmware);
        if (err) {
            log_error(err, "Error opening the receiver\n");
            receiver_close(&rctx);
            continue;
        }
        log_info("Receiving firmware chunks\n");
        while (!rctx.firmware_received) {
            err = receiver_chunk(&rctx);
            if (err) {
                log_error(err, "Error receiving firmware chunk\n");
                break;
            };
            COAP_SEND(txp);
            if (!rctx.firmware_received) {
                log_debug("Setting the timer for 10 seconds\n");
                etimer_set(&et, (CLOCK_SECOND*20));
                PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            }
        }
        log_info("Firmware received\n");

        err = receiver_close(&rctx);
        if (err) {
            log_error(err, "Error closing the reciver\n");
            continue;
        }
        if (!rctx.firmware_received) {
            log_debug("Error receving firmware...restarting\n");
            continue;
        }
        watchdog_stop();
        uint8_t buffer[BUFFER_LEN];
#ifdef WITH_CRYPTOAUTHLIB
        ATCA_STATUS status = atcab_init(&cfg_ateccx08a_i2c_default);
        if (status != ATCA_SUCCESS) {
            log_error(GENERIC_ERROR, "Failure initializing ATECC508A\n");
        }
#endif
        err = verify_object(&new_firmware, df, x, y, ef, buffer, BUFFER_LEN);
#ifdef WITH_CRYPTOAUTHLIB
        if (!err) {
            atcab_release();
        }
 #endif
        if (err) {
            log_info("Verification failed\n");
            err = invalidate_object(id, &obj_t);
            if (err) {
                log_error(err, "Error invalidating object");
            }
            continue;
        }
        watchdog_start();
        log_info("Received image is valid\n");
        break;
    }
    unsubscribe(&sctx);
    txp_end(&txp);
    watchdog_reboot();
    PROCESS_END();
}
