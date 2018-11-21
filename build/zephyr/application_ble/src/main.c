#include <zephyr.h>
#include <string.h>
#include <stdio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>

#include <libpull/network/gatt.h>

#define DEVICE_NAME	CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN	(sizeof(DEVICE_NAME) - 1)

#if  WITH_TINYCRYPT
int default_CSPRNG(uint8_t *dest, unsigned int size) {
    return 0;
}
#endif

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0a, 0x18),
};

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

struct bt_conn *default_conn;

static void connected(struct bt_conn *conn, uint8_t err) {
	if (err) {
		printf("Connection failed (err %u)\n", err);
	} else {
        printf("Max MTU: %u\n", bt_gatt_get_mtu(conn));
        default_conn = bt_conn_ref(conn);
		printf("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason) {
	printf("Disconnected (reason %u)\n", reason);
    if (default_conn) {
        bt_conn_unref(default_conn);
        default_conn = NULL;
    }
}

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
};

int main(void) {
	int err = bt_enable(NULL);
	if (err) {
		printf("Bluetooth init failed (err %d)\n", err);
		return 1;
	}

	printf("Bluetooth initialized\n");

    mem_object_t obj;
    libpull_gatt_init(&obj);

	bt_conn_cb_register(&conn_callbacks);
	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));

	if (err) {
		printf("Advertising failed (err %d)\n", err);
		return 1;
	}
    printf("Advertising...");
    return 0;
}

