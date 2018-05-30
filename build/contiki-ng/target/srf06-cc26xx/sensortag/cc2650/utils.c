#include "utils.h"
#include "driverlib/flash.h"
#include "driverlib/vims.h"

void flash_write_protect() {
    uint32_t page = 0;
    for (page=BOOTLOADER_START_PAGE; page<=IMAGE_END_PAGE; page++) {
        FlashProtectionSet(page, FLASH_WRITE_PROTECT);
    }
}
