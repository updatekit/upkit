#include <string.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>

#include <libpull/common.h>
#include <libpull/network/writer.h>

#include "platform_headers.h"

#include "libpull_gatt.h"

push_receiver_ctx_t ctx;

/* Libpull GATT service UUIDs */
static struct bt_uuid_128 libpull_service_udid =
BT_UUID_INIT_128(LIBPULL_SERVICE_UUID);

static const struct bt_uuid_128 libpull_version =
BT_UUID_INIT_128(LIBPULL_VERSION_UUID);

static const struct bt_uuid_128 libpull_platform =
BT_UUID_INIT_128(LIBPULL_PLATFORM_UUID);

static const struct bt_uuid_128 libpull_identity =
BT_UUID_INIT_128(LIBPULL_IDENTITY_UUID);

static const struct bt_uuid_128 libpull_update =
BT_UUID_INIT_128(LIBPULL_UPDATE_UUID);

static const struct bt_uuid_128 libpull_state =
BT_UUID_INIT_128(LIBPULL_STATE_UUID);

static const struct bt_uuid_128 libpull_result =
BT_UUID_INIT_128(LIBPULL_RESULT_UUID);

static const struct bt_uuid_128 libpull_received =
BT_UUID_INIT_128(LIBPULL_RECEIVED_UUID);

static const struct bt_uuid_128 libpull_image =
BT_UUID_INIT_128(LIBPULL_IMAGE_UUID);

static ssize_t gatt_read_uint8(struct bt_conn *conn,
        const struct bt_gatt_attr *attr, void *buf,
        uint16_t len, uint16_t offset) {
    void *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
            sizeof(uint8_t));
}
static ssize_t gatt_read_uint16(struct bt_conn *conn,
        const struct bt_gatt_attr *attr, void *buf,
        uint16_t len, uint16_t offset) {
    void *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
            sizeof(uint16_t));
}
static ssize_t gatt_read_uint32(struct bt_conn *conn,
        const struct bt_gatt_attr *attr, void *buf,
        uint16_t len, uint16_t offset) {
    void *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
            sizeof(uint32_t));
}

static ssize_t gatt_read_identity(struct bt_conn *conn,
        const struct bt_gatt_attr *attr, void *buf,
        uint16_t len, uint16_t offset) {
    void *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
            sizeof(identity_t));
}

static ssize_t libpull_fsm(struct bt_conn *conn,
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
    /* 1 READ ONLY version */
    BT_GATT_CHARACTERISTIC(&libpull_version.uuid,
            BT_GATT_CHRC_READ,
            BT_GATT_PERM_READ,
            gatt_read_uint16, NULL, &ctx.version),
    /* 2 READ ONLY platform */
    BT_GATT_CHARACTERISTIC(&libpull_platform.uuid,
            BT_GATT_CHRC_READ,
            BT_GATT_PERM_READ,
            gatt_read_uint16, NULL, &ctx.platform),
    /* 3 READ ONLY identity */
    BT_GATT_CHARACTERISTIC(&libpull_identity.uuid,
            BT_GATT_CHRC_READ,
            BT_GATT_PERM_READ,
            gatt_read_identity, NULL, &ctx.identity),
    /* 4 WRITE ONLY update */
    BT_GATT_CHARACTERISTIC(&libpull_update.uuid,
            BT_GATT_CHRC_WRITE,
            BT_GATT_PERM_WRITE,
            NULL, libpull_fsm, &update_characteristic),
    /* READ ONLY state */
    BT_GATT_CHARACTERISTIC(&libpull_state.uuid,
            BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
            BT_GATT_PERM_READ,
            gatt_read_uint8, NULL, &ctx.state),
    BT_GATT_CCC(notify_state, notify_state_cb),
    /* 7 READ ONLY result */
    BT_GATT_CHARACTERISTIC(&libpull_result.uuid,
            BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
            BT_GATT_PERM_READ,
            gatt_read_uint8, NULL, &ctx.result),
    BT_GATT_CCC(notify_result, notify_result_cb), /* 8 */
    /* 9 READ ONLY written bytes */
    BT_GATT_CHARACTERISTIC(&libpull_received.uuid,
            BT_GATT_CHRC_READ,
            BT_GATT_PERM_READ,
            gatt_read_uint32, NULL, &ctx.received),
    /* 10 WRITE ONLY firmware */
    BT_GATT_CHARACTERISTIC(&libpull_image.uuid,
            BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
            BT_GATT_PERM_WRITE,
            NULL, libpull_fsm, &image_characteristic),
};

static const uint8_t update_cmd = UPDATE_CMD_VAL;

static ssize_t libpull_fsm(struct bt_conn *conn,
        const struct bt_gatt_attr *attr,
        const void *buf, uint16_t len,
        uint16_t offset, uint8_t flags) {
    uint8_t* characteristic = attr->user_data;
    uint8_t old_state = ctx.state;
    uint8_t old_result = ctx.result;
    pull_error err;
    libpull_cmd_t cmd = (*characteristic == update_characteristic)? CMD_UPDATE: CMD_PACKAGE;
    err = push_receiver_receive(&ctx, cmd, buf, len);
    if (err) {
        log_error(err, "Error in the fsm\n");
        return 0;
    }
    if (notify_state_flag && (old_state != ctx.state)) {
        bt_gatt_notify(NULL, &libpull_attrs[10], &ctx.state, sizeof(uint8_t));
    }
    if (notify_result_flag && (old_result != ctx.result)) {
        bt_gatt_notify(NULL, &libpull_attrs[13], &ctx.result, sizeof(uint8_t));
    }
    return len;
}

static struct bt_gatt_service libpull_service = BT_GATT_SERVICE(libpull_attrs);

pull_error libpull_gatt_init(mem_object_t *obj) {
    pull_error err = push_receiver_init(&ctx, obj);
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
