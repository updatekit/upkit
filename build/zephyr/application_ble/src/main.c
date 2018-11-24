#include <zephyr.h>
#include <string.h>
#include <stdio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>

#include <libpull/network/gatt.h>
#include "platform_headers.h"
#include "default_keys.h"

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

int main(void) {
	if (bt_enable(NULL)) {
		log_debug("Bluetooth init failed (err %d)\n", err);
		return 1;
	}

    mem_object_t obj;
    libpull_gatt_init(&safestore_g, &obj);

	if (bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd))) {
		log_debug("Advertising failed (err %d)\n", err);
		return 1;
	}
    log_debug("Advertising...");
    return 0;
}

