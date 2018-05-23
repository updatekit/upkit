#include "common/libpull.h"
#include "network/receiver.h"
#include "network/subscriber.h"
#include "network/async.h"
#include "security/ecc.h"
#include "security/sha256.h"
#include "security/verifier.h"
#include "memory/memory_objects.h"
#include "memory/memory.h"
#include "memory/manifest.h"

#include "memory_file_posix.h"
#include "transport_libcoap.h"

#include "support/sample_data.h"
#include "support/test_runner.h"
#include "unity.h"
#include <unistd.h>


/* All this variables will be used by a stackless function
 * so it is important to make sure that they are global.
 * To avoid errors do not define variables inside of the
 * function itself*/

static txp_ctx stxp;
static subscriber_ctx sctx;
static receiver_ctx rctx;
static txp_ctx rtxp;
static obj_id id;
static mem_object new_obj;
static mem_object obj_t;
static uint8_t buffer[BUFFER_SIZE];

static pull_error err;
static agent_status_t agent;

agent_t update_agent(update_agent_config* cfg) {

    // (1) Init transport interface
    PULL_INIT(STATE_INIT);
    err = txp_init(&stxp, cfg->endpoint, cfg->port, cfg->connection_type, cfg->conn_data);
    if (err) {
        log_error(err, "Error initializing the transport interface\n");
        PULL_RETURN(STATE_INIT, err, FAILURE);
    }

    // (2) Subscribe to the server
    PULL_CONTINUE(STATE_SUBSCRIBE, PULL_SUCCESS, CONTINUE);
    err = subscribe(&sctx, &stxp, cfg->resource, &obj_t);
    if (err) {
        log_error(err, "Error subscribing to the server\n");
        PULL_RETURN(STATE_SUBSCRIBE, err, FAILURE);
    }

    // (3) Check if there are updates
    PULL_CONTINUE(STATE_CHECKING_UPDATES, PULL_SUCCESS, CONTINUE);
    log_info("Checking for updates\n");
    while (!sctx.has_updates) {
        err = check_updates(&sctx, subscriber_cb);
        if (err) {
            log_error(err, "Error setting check udpates callback\n");
            PULL_CONTINUE(STATE_CHECKING_UPDATES_FAILURE, err, FAILURE);
        }
        //loop_once(&stxp, 1000);
        PULL_CONTINUE(STATE_CHECKING_UPDATES_SEND, PULL_SUCCSS, TXP_SEND);
    }
    log_info("An update is available\n");

    // (4) Searching slot
    PULL_CONTINUE(STATE_SEARCHING_SLOT, PULL_SUCCESS, CONTINUE);
    uint16_t version;
    err = get_oldest_firmware(&id, &version, &obj_t);
    if (err) {
        log_error(err, "Error while getting the oldest slot\n");
        PULL_RETURN(STATE_SEARCHING_SLOT, err, FAILURE);
    }G
    err = memory_open(&new_obj, id, WRITE_ALL);
    if (err) {
        log_error(err, "Error opening the memory_object\n");
        PULL_RETURN(STATE_SEARCHING_SLOT, err, FAILURE);
    }

    // (5) Connection to receiver server
    PULL_CONTINUE(STATE_CONN_RECEIVER, PULL_SUCCESS, CONTINUE);
    if (cfg->reuse_connection) {
        rtxp = stxp;
    } else { 
        err = txp_init(&rtxp, cfg->receiver.endpoint,
                    cfg->receiver.port, cfg->receiver.connection_type, 
                    cfg->receiver.conn_data);
        if (err) {
            log_error(err, "Error while connecting to receiver server\n");
            PULL_RETURN(STATE_CONN_RECEIVER_FAILURE, err, FAILURE);
        }
    }
    err = receiver_open(&rctx, &rtxp, identity_g, cfg->receiver.resource, &new_obj);
    if (err) {
        log_error(err, "Error opening the receiver\n");
        PULL_RETURN(STATE_CONN_RECEIVER_FAILURE, err, FAILURE);
    }

    // (5) Receiving update
    PULL_CONTINUE(STATE_RECEIVE, PULL_SUCESS, CONTINUE);
    while (!rctx.firmware_received) {
        err = receiver_chunk(&rctx);
        if (err) {
            log_error(err, "Error receiving chunk\n");
            PULL_CONTINUE(STATE_RECEIVE_FAILURE, err, FAILURE);
        }
        //loop(&rtxp, 1000);
        PULL_CONTINUE(STATE_RECEIVE_SEND, PULL_SUCESS, SEND);
    }
    txp_end(&rtxp);
    err = receiver_close(&rctx); // Check this error

    // (6) Verify received firmware
    PULL_CONTINUE(STATE_VERIFY, PULL_SUCCESS, CONTINUE);
    err = verify_object(&new_obj, cfg->df, vendor_x_g, vendor_y_g, cfg->ef, 
            cfg->buffer, cfg->buffer_size);
    if (err) {
        log_error(err, "Verification failed\n");
        PULL_RETURN(STATE_VERIFY, err, FAILURE);
    }

    // (7) Final
    PULL_CONTINUE(STATE_FINAL, PULL_SUCCESS, CONTINUE);
    err = memory_close(&new_obj);
    err = unsubscribe(&sctx);
    err = txp_end(&stxp);
    PULL_FINISH();
}

