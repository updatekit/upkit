#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <zephyr.h>
#include <stdio.h>
#include <stdlib.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>

#include <libpull/common.h>
#include <libpull/network/writer.h>
#include <libpull/memory.h>

#include "platform_headers.h"

#include "libpull_gatt.h"

/* Libpull GATT service UUID */
static struct bt_uuid_128 libpull_service_udid = 
        BT_UUID_INIT_128(LIBPULL_SERVICE_UUID);

static const struct bt_uuid_128 libpull_version = 
        BT_UUID_INIT_128(LIBPULL_VERSION_UUID);

static const struct bt_uuid_128 libpull_state = 
        BT_UUID_INIT_128(LIBPULL_STATE_UUID);

static const struct bt_uuid_128 libpull_packets = 
        BT_UUID_INIT_128(LIBPULL_PACKETS_UUID);


static const struct bt_uuid_128 libpull_cmd = 
        BT_UUID_INIT_128(LIBPULL_CMD_UUID);

static const struct bt_uuid_128 libpull_firmware = 
        BT_UUID_INIT_128(LIBPULL_FIRMWARE_UUID);

static libpull_state_t state = STATE_STOP;
static libpull_cmd_t command = CMD_YIELD;
static uint16_t version = 0x1;
static uint16_t packets = 0x0;

static mem_object_t obj;
static struct writer_ctx_t wctx;
pull_error validate_cb(manifest_t* mt, void* user_data) {
    printf("Validating received data\n");
    return PULL_SUCCESS;
}

static struct bt_gatt_cep firmware_cep = {
    .properties = BT_GATT_CEP_RELIABLE_WRITE,
};

static ssize_t libpull_firmware_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
        const void *buf, uint16_t len, uint16_t offset, uint8_t flags);

static ssize_t libpull_version_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
        void *buf, uint16_t len, uint16_t offset) {
    const char *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
            sizeof(version_t));
}

static ssize_t libpull_state_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
        void *buf, uint16_t len, uint16_t offset) {
    const char *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
            sizeof(libpull_state_t));
}

static ssize_t libpull_packets_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
        void *buf, uint16_t len, uint16_t offset) {
    const char *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
            sizeof(packets));
}

void manage_command();

static ssize_t libpull_command_write(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags);




static struct bt_gatt_attr libpull_attrs[] = {
    /* Vendor Primary Service Declaration */
    BT_GATT_PRIMARY_SERVICE(&libpull_service_udid),
    /* First characteristic READ ONLY version */
    BT_GATT_CHARACTERISTIC(&libpull_version.uuid,
            BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
            BT_GATT_PERM_READ,
            libpull_version_read, NULL, &version),
    /* Second characteristic READ ONLY state */
    BT_GATT_CHARACTERISTIC(&libpull_state.uuid,
            BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
            BT_GATT_PERM_READ,
            libpull_state_read, NULL, &state),
    /* Thids characteristic READ ONLY command */
    BT_GATT_CHARACTERISTIC(&libpull_cmd.uuid,
            BT_GATT_CHRC_WRITE,
            BT_GATT_PERM_WRITE,
            NULL, libpull_command_write, &command),
    /* Fourth characteristic READ ONLY */
    BT_GATT_CHARACTERISTIC(&libpull_packets.uuid,
            BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
            BT_GATT_PERM_READ,
            libpull_packets_read, NULL, &packets),
    /* Fourth characteristic WRITE ONLY firmware */
    BT_GATT_CHARACTERISTIC(&libpull_firmware.uuid, 
            BT_GATT_CHRC_WRITE | BT_GATT_CHRC_EXT_PROP| BT_GATT_CHRC_WRITE_WITHOUT_RESP,
            BT_GATT_PERM_WRITE | BT_GATT_PERM_PREPARE_WRITE,
            NULL, libpull_firmware_write, &wctx),
    BT_GATT_CEP(&firmware_cep)
};

static ssize_t libpull_command_write(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    u8_t *value = attr->user_data;

    if (offset + len > sizeof(libpull_cmd_t)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    memcpy(value + offset, buf, len);

    manage_command();
    bt_gatt_notify(NULL, &libpull_attrs[2], &state, sizeof(state));
    return len;
}

static ssize_t libpull_firmware_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
        const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    if (state != STATE_SEND) {
        printf("State was not correct\n");
        return 0;
    }
    
    if (writer_chunk(&wctx, buf, len) != PULL_SUCCESS) {
        printf("Error writing chunk\n\n");
        return 0;
    }
    packets++;
    if ((packets % 50) == 0) {
        printf("sending notification\n\n");
        bt_gatt_notify(NULL, &libpull_attrs[4], &packets, sizeof(packets));
    }

    return len;
}

void manage_command() {
    pull_error err;
    switch (command) {
        case CMD_INIT:
            err = memory_open(&obj, OBJ_2, WRITE_ALL);
            if (err) {
                log_error(err, "Error opening the memory_object\n");
                return;
            }
            err = writer_open(&wctx, &obj, validate_cb, NULL);
            if (err) {
                log_error(err, "Error opening the memory_object\n");
                return;
            }
            state = STATE_SEND;
            break;
        default:
            printf("Invalid command\n");
            command = CMD_YIELD;
    }
}

static struct bt_gatt_service libpull_service = BT_GATT_SERVICE(libpull_attrs);

void libpull_gatt_init() {
    bt_gatt_service_register(&libpull_service);
}
