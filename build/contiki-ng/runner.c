#include "contiki.h"

#include "contiki-lib.h"
#include "dev/watchdog.h"
#include "leds.h"

#include "common/logger.h"
#include "memory/memory_objects.h"
#include "network/receiver.h"
#include "network/subscriber.h"
#include "transport/transport_ercoap.h"
#include "memory_headers.h"
#include "default_configs.h"
#include "security/verifier.h"
#include "security/sha256.h"

static const uint8_t x[32] = {
    0x8b,0x27,0x39,0x67,0x01,0x4b,0x1c,0xae,0xfe,0x8a,0x18,0x6e,0xea,0x27,0x86,0x34,
    0x0e,0xea,0x35,0x3d,0x8c,0x65,0xf6,0x59,0xfc,0xcb,0x23,0xd7,0xfa,0xab,0x7b,0x18
};
static const uint8_t y[32] = {
    0x14,0x75,0x33,0xec,0x17,0xb7,0x54,0x50,0xca,0x98,0x35,0xad,0x58,0xbe,0xd5,0xfa,
    0x48,0xbc,0xa0,0x24,0x81,0xba,0xfa,0x3d,0xcd,0x8d,0x5a,0x7f,0x40,0xbc,0x70,0x94
};

#define BUFFER_LEN 0x100

PROCESS(update_process, "OTA Update process");
PROCESS(main_process, "Main process");
AUTOSTART_PROCESSES(&update_process, &main_process);

txp_ctx txp;
subscriber_ctx sctx;
receiver_ctx rctx;

obj_id id;
mem_object obj_t;
static struct etimer et;
void* conn_data;

static struct etimer et_led;
PROCESS_THREAD(main_process, ev, data) {
    PROCESS_BEGIN();
    do {
        leds_toggle(LEDS_RED);
        etimer_set(&et_led, (CLOCK_SECOND*1));
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et_led));
    } while (1);
    PROCESS_END();
}

PROCESS_THREAD(update_process, ev, data) {
    PROCESS_BEGIN();
#ifdef CONN_UDP
    conn_type type = TXP_UDP;
    conn_data = NULL;
#elif CONN_DTLS_ECDSA
    conn_type type = TXP_DTLS_ECDSA;
    conn_data = &dtls_ecdsa_data;
#elif CONN_DTLS_PSK
    conn_type type = TXP_DTLS_PSK;
    conn_data = &dtls_psk_data;
#endif

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
        // download the image to the oldest slot
        err = receiver_open(&rctx, &txp, "firmware", id, &obj_t);
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
            log_info("Error receving firmware...restarting\n");
            continue;
        }
        metadata mt;
        err = read_firmware_metadata(id, &mt, &obj_t);
        if (err) {
            log_error(err, "Failure reading firmware metadata, aborting\n");
            memset(&mt, 0x0, sizeof(metadata));
            write_firmware_metadata(id, &mt, &obj_t);
            continue;
        }
        print_metadata(&mt);
        watchdog_stop();
        uint8_t buffer[BUFFER_LEN];
        digest_func digest;
#ifdef WITH_CRYPTOAUTHLIB
        digest = cryptoauthlib_digest_sha256;
        ATCA_STATUS status = atcab_init(&cfg_ateccx08a_i2c_default);
        if (status != ATCA_SUCCESS) {
            log_error(GENERIC_ERROR, "Failure initializing ATECC508A\n");
        }
#endif
        err = verify_object(id, digest, x, y, secp256r1, &obj_t, buffer, BUFFER_LEN);
#ifdef WITH_CRYPTOAUTHLIB
        if (status == ATCA_SUCCESS) {
            atcab_release();
        }
 #endif
        if (err) {
            log_warn("Verification failed\n");
            memset(&mt, 0x0, sizeof(metadata));
            write_firmware_metadata(id, &mt, &obj_t);
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
