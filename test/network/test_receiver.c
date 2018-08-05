#include <libpull/common.h>
#include <libpull/network.h>
#include <libpull/memory.h>

#include "support/support.h"

#define PROV_SERVER "localhost"
mem_object_t obj;

void ntest_prepare(void) {
    restore_assets();
    nTEST_TRUE(memory_open(&obj, OBJ_A, WRITE_ALL) == PULL_SUCCESS);
}

void ntest_clean(void) {
    manifest_t invalid_mt;
    /* Restore state */
    bzero(&invalid_mt, sizeof(manifest_t));
    pull_error err = write_firmware_manifest(&obj, &invalid_mt);
    nTEST_TRUE(!err);
}

void test_get_firmware(void) {
    conn_ctx conn;
    receiver_ctx rcv;
    pull_error err = conn_init(&conn, PROV_SERVER, 0, PULL_UDP, NULL);
    nTEST_TRUE(!err);
    err = receiver_open(&rcv, &conn, &identity_g, "firmware", &obj);
    nTEST_TRUE(!err);
    while (!rcv.firmware_received) {
        err = receiver_chunk(&rcv);
        nTEST_TRUE(!err);
        loop(&conn, 1000);
    }
    err = receiver_close(&rcv);
    nTEST_TRUE(!err);
    conn_end(&conn);
}

void test_get_firmware_dtls(void) {
    conn_ctx conn;
    receiver_ctx rcv;
    dtls_ecdh_data_t ecdh_data = {
        .priv_key = (uint8_t *) dtls_client_priv_g,
        .pub_key_x = (uint8_t *) dtls_client_x_g,
        .pub_key_y = (uint8_t *) dtls_client_y_g
    };
    pull_error err = conn_init(&conn, PROV_SERVER, 0, PULL_DTLS_ECDH, &ecdh_data);
    nTEST_TRUE(!err);
    err = receiver_open(&rcv, &conn, &identity_g, "firmware", &obj);
    printf("Starting receiving the firmware\n");
    while (!rcv.firmware_received) {
        err = receiver_chunk(&rcv);
        nTEST_TRUE(!err);
        loop(&conn, 1000);
    }
    printf("firmware downloaded\n");
    err = receiver_close(&rcv);
    nTEST_TRUE(!err);
    conn_end(&conn);
}

void test_receiver_chunk_invalid_connection(void) {
    conn_ctx conn;
    receiver_ctx rcv;
    pull_error err = conn_init(&conn, PROV_SERVER, 0, PULL_UDP, NULL);
    nTEST_TRUE(!err);
    err = receiver_open(&rcv, &conn, &identity_g, "firmware", &obj);
    nTEST_TRUE(!err);
    rcv.conn = NULL;
    err = receiver_chunk(&rcv);
    nTEST_TRUE(err);
}

void test_get_firmware_invalid_resource(void) {
    conn_ctx conn;
    receiver_ctx rcv;
    pull_error err = conn_init(&conn, PROV_SERVER, 0, PULL_UDP, NULL);
    nTEST_TRUE(!err);
    err = receiver_open(&rcv, &conn, &identity_g, "antani", &obj);
    nTEST_TRUE(!err);
    while (!rcv.firmware_received && !err) {
        err = receiver_chunk(&rcv);
        if (err) {
            continue; // This should be applied everywhere
        }
        loop(&conn, 1000);
    }
    nTEST_TRUE(err == NETWORK_ERROR, "%s\n", err_as_str(err));
}

void test_get_firmware_invalid_size(void) {
    conn_ctx conn;
    receiver_ctx rcv;
    pull_error err = conn_init(&conn, PROV_SERVER, 0, PULL_UDP, NULL);
    nTEST_TRUE(!err);
    err = receiver_open(&rcv, &conn, &identity_g, "firmware/invalid_size", &obj);
    nTEST_TRUE(!err);
    while (!rcv.firmware_received && !err) {
        err = receiver_chunk(&rcv);
        if (err) {
            continue; // This should be applied everywhere
        }
        loop(&conn, 1000);
    }
    nTEST_TRUE(err == INVALID_SIZE_ERROR, "%s\n", err_as_str(err));
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_get_firmware);
    nTEST_RUN(test_receiver_chunk_invalid_connection);
    nTEST_RUN(test_get_firmware_invalid_resource);
    nTEST_RUN(test_get_firmware_dtls);
    nTEST_END();
    nTEST_RETURN();
}
