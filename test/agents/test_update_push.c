#include <libpull/security.h>
#include <libpull/network.h>
#include "libpull_agents/update.h"
#include "support/support.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_LEN 1024

/**
 * This test simulates the behaviour of a push interface,
 * such as when using BLE, and data is pushed from the
 * server to the device itself.
 */
static bool success = false;
int fd;

void ntest_prepare(void) {
    success = false;
    fd = open("../assets/slot_d.bin", O_RDONLY); 
    nTEST_TRUE(fd > 0);
}

void ntest_clean(void) { 
    restore_assets();
    close(fd);
}

void test_update_success(void) {
    pull_error err;
    fsm_ctx_t fsmc;
    mem_object_t obj;
    err = fsm_init(&fsmc, &safestore_g, &obj);
    nTEST_TRUE(!err);

    // 1) CB: Server reads version
    version_t client_version = fsmc.version;
    nTEST_TRUE(client_version == 0xb);

    // 2) Server cheks that the version is higher
    manifest_t mt;
    int res = read(fd, &mt, sizeof(manifest_t));
    nTEST_TRUE(res == sizeof(manifest_t));

    nTEST_TRUE(get_version(&mt) > client_version);

    // 3) CB: To start the update write the new version
    version_t new_version = get_version(&mt);
    err = fsm(&fsmc, &new_version, sizeof(version_t));
    nTEST_TRUE(!err);

    // 4) CB: The servers reads state until it changes
    nTEST_TRUE(fsmc.state == STATE_START_UPDATE);

    // 5) CB: When the state changes it reads the identity data
    receiver_msg_t msg;
    err = fsm(&fsmc, &msg, sizeof(msg));
    nTEST_TRUE(!err);

    // 6) Creates the update with that identity data
    set_udid(&mt, msg.udid);
    set_diff_version(&mt, 0);
    set_nonce(&mt, msg.nonce);
    uint8_t buffer[BUFFER_LEN];
    sign_manifest_server(&mt, server_priv_g, buffer);

    // 7) CB: Starts sending the udpate
    printf("%d\n", get_version(&mt));
    nTEST_TRUE(fsmc.state == STATE_RECEIVE_MANIFEST);
    err = fsm(&fsmc, &mt, sizeof(manifest_t));
    nTEST_TRUE(!err);

    // 8) Waits until the fsm changes state
    nTEST_TRUE(fsmc.state == STATE_RECEIVE_FIRMWARE);

    // 9) Continues sending the firmware
    uint8_t buf[100];
    int ret, len = (rand()%50)+50;
    while (ret = read(fd, &buf, len)) {
        err = fsm(&fsmc, buf, ret);
        nTEST_TRUE(!err);
        if (ret != len) {
            break;
        }
        len = (rand()%50)+50;
    }

    // 9) Wait until the fsm changes state
    nTEST_TRUE(fsmc.state == STATE_REBOOT);
}

int main() {
    nTEST_INIT();
    nTEST_RUN(test_update_success);
    nTEST_END();
    nTEST_RETURN();
}
