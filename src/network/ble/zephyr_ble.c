#ifdef WITH_ZEPHYR_BLE

#include <string.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>

#include <libpull/common.h>
#include <libpull/network.h>

#include "platform_headers.h"

fsm_ctx_t fsmc;

/* Libpull GATT service UUIDs */
static struct bt_uuid_128 libpull_service_udid =
    BT_UUID_INIT_128(LIBPULL_SERVICE_UUID);

static const struct bt_uuid_128 libpull_version_read =
    BT_UUID_INIT_128(LIBPULL_VERSION_READ_UUID);

static const struct bt_uuid_128 libpull_version_write =
    BT_UUID_INIT_128(LIBPULL_VERSION_WRITE_UUID);

static const struct bt_uuid_128 libpull_receiver_msg =
    BT_UUID_INIT_128(LIBPULL_RECEIVER_MSG_UUID);

static const struct bt_uuid_128 libpull_state =
    BT_UUID_INIT_128(LIBPULL_STATE_UUID);

static ssize_t gatt_read_state(struct bt_conn *conn,
        const struct bt_gatt_attr *attr, void *buf,
        uint16_t len, uint16_t offset) {
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &fsmc->state, sizeof(fsm_state_t));
}

static ssize_t gatt_read_version(struct bt_conn *conn,
        const struct bt_gatt_attr *attr, void *buf,
        uint16_t len, uint16_t offset) {
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &fsmc->version,
            sizeof(version_t));
}

static ssize_t gatt_write_version(struct bt_conn *conn,
        const struct bt_gatt_attr *attr, void *buf,
        uint16_t len, uint16_t offset) {
    return 0; // TODO
}

static ssize_t gatt_read_receiver_msg(struct bt_conn *conn,
        const struct bt_gatt_attr *attr, void *buf,
        uint16_t len, uint16_t offset) {
    void *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
            sizeof(receiver_msg));
}

static ssize_t gatt_write_image(struct bt_conn *conn,
        const struct bt_gatt_attr *attr,
        const void *buf, uint16_t len,
        uint16_t offset, uint8_t flags);

static uint8_t update_characteristic = 0x0;
static uint8_t image_characteristic = 0x1;

static struct bt_gatt_ccc_cfg notify_state[BT_GATT_CCC_MAX] = {};
static struct bt_gatt_ccc_cfg notify_result[BT_GATT_CCC_MAX] = {};

uint8_t notify_state_flag = 0;
uint8_t notify_result_flag = 0;

static void notify_state_cb(const struct bt_gatt_attr *attr, u16_t value) {
    printf("received request for notify state\n");
    notify_state_flag = (value == BT_GATT_CCC_NOTIFY)? 1 : 0;
}
static void notify_result_cb(const struct bt_gatt_attr *attr, u16_t value) {
    printf("received request for notify result\n");
    notify_result_flag = (value == BT_GATT_CCC_NOTIFY)? 1 : 0;
}

static struct bt_gatt_attr libpull_attrs[] = {
    /* 0 Vendor Primary Service Declaration */
    BT_GATT_PRIMARY_SERVICE(&libpull_service_udid),
    /* 1 READ_ONLY version */
    BT_GATT_CHARACTERISTIC(&libpull_version_read.uuid,
            BT_GATT_CHRC_READ,
            BT_GATT_PERM_READ,
            gatt_read_version, NULL, NULL),
    /* 2 WRITE_ONLY version */
    BT_GATT_CHARACTERISTIC(&libpull_version_write.uuid,
            BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
            BT_GATT_PERM_WRITE,
            NULL, gatt_write_version, NULL),
    /* 3 READ_ONLY receiver_msg */
    BT_GATT_CHARACTERISTIC(&libpull_receiver_msg.uuid,
            BT_GATT_CHRC_READ,
            BT_GATT_PERM_READ,
            gatt_read_receiver_msg, NULL, &ctx.receiver_msg),
    /* 4 WRITE_ONLY data */
    BT_GATT_CHARACTERISTIC(&libpull_image.uuid,
            BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
            BT_GATT_PERM_WRITE,
            NULL, gatt_write_image, &image_characteristic),
    /* 5 READ_ONLY & NOTIFY state */
    BT_GATT_CHARACTERISTIC(&libpull_state.uuid,
            BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
            BT_GATT_PERM_READ,
            gatt_read_state, NULL, NULL),
    BT_GATT_CCC(notify_state, notify_state_cb),
};

static const uint8_t update_cmd = UPDATE_CMD_VAL;

static ssize_t gatt_write_image(struct bt_conn *conn,
        const struct bt_gatt_attr *attr,
        const void *buf, uint16_t len,
        uint16_t offset, uint8_t flags) {
    uint8_t* characteristic = attr->user_data;
    uint8_t old_state = ctx.state;
    err = libpull_fsm_receive(&ctx, buf, len);
    if (err) {
        log_error(err, "Error in the fsm\n");
        return 0;
    }
    if (notify_state_flag && (old_state != ctx.state)) {
        bt_gatt_notify(NULL, &libpull_attrs[10], &ctx.state, sizeof(uint8_t));
    }
    return len;
}

static struct bt_gatt_service libpull_service = BT_GATT_SERVICE(libpull_attrs);

pull_error libpull_gatt_init(mem_object_t *obj) {
    pull_error err = fsm_init(&ctx, obj);
    if (err) {
        log_error(err, "Error initializing push receiver\n");
        return err;
    }
    if (bt_gatt_service_register(&libpull_service) != 0) {
        log_error(err, "Error registering gatt service\n");
        return RECEIVER_OPEN_ERROR;
    }
    return PULL_SUCCESS;
}

#endif /* WITH_ZEPHYR_BLE */
