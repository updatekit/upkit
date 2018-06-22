#!/bin/bash
# This script creates an ota image, containing the bootloader,
# the manifest and the binary image
set -e

# Include the configuration file
BOARD=nrf52840_pca10056
source board/$BOARD/Makefile.conf

ROOTDIR=$(cd $(dirname $0) && pwd -P)

# Default paths
FIRMWARE_TOOL_DIR="$ROOTDIR/../../utils/firmware_tool/"
FIRMWARE_TOOL="$FIRMWARE_TOOL_DIR/firmware_tool"
FIRMWARE_DIR="$ROOTDIR/firmware"

# Configurations
BOOTLOADER="$ROOTDIR/bootloader/build/zephyr/zephyr.bin"
BOOTLOADER_CTX="$ROOTDIR/bootloader_ctx/bootloader_ctx.bin"
MANIFEST="$FIRMWARE_DIR/manifest.bin"
IMAGE="$ROOTDIR/application/build/zephyr/zephyr.bin"
FIRMWARE="$ROOTDIR/firmware/firmware.bin"

check_args() {
    if [ ! -f "$IMAGE" ]; then
        echo "You should first build the binary image";
        exit 1;
    fi
    if [[ ! -f "$BOOTLOADER" ]]; then
        echo "Bootloader not found, you should first build it..."
        exit 1
    fi
    if [ ! -f $BOOTLOADER_CTX ]; then
        echo "Bootloader ctx not present. Exiting..."
        exit 1;
    fi
}

check_firmware_tool() {
    if [ ! -d $FIRMWARE_DIR ]; then
        echo "Creating directory $FIRMWARE_DIR..."
        mkdir $FIRMWARE_DIR
        echo "Creating directory $FIRMWARE_DIR...done"
    fi

    if [ ! -f $FIRMWARE_TOOL ]; then
        echo "Build firmware tool..."
        (cd $FIRMWARE_TOOL_DIR && make);
        echo "Build firmware tool...done"
    fi
}

generate_manifest() {
    echo "Generating firmware manifest..."
    (( IMAGE_START_OFFSET=$IMAGE_START_PAGE*$PAGE_SIZE+$MANIFEST_SIZE ))
    (( IMAGE_END_OFFSET=$IMAGE_END_PAGE*$PAGE_SIZE))
    srec_cat $IMAGE -binary \
         -crop $IMAGE_START_OFFSET $IMAGE_END_OFFSET \
         -offset -$IMAGE_START_OFFSET -o $FIRMWARE.tmp -binary
    ($FIRMWARE_TOOL manifest generate -vv)
    echo "Generating firmware manifest...done"
}

generate_ota_image() {
    echo "Adding the manifest to the firmware..."

    srec_cat $MANIFEST -binary $FIRMWARE.tmp -binary \
        -offset $MANIFEST_SIZE -o $FIRMWARE -binary
    echo "Adding the manifest to the firmware...done"
}

generate_flashable_firmware() {
    echo "Adding the bootloader to the firmware..."
    # I assume the bootloader start offset as 0 since is the default address to
    # load instructions
    (( BOOTLOADER_START_OFFSET=$BOOTLOADER_START_PAGE*$PAGE_SIZE ))
    (( BOOTLOADER_END_OFFSET=$BOOTLOADER_END_PAGE*$PAGE_SIZE ))
    (( IMAGE_START_OFFSET=$IMAGE_START_PAGE*$PAGE_SIZE ))
    (( IMAGE_END_OFFSET=$IMAGE_END_PAGE*$PAGE_SIZE ))
    cmd="srec_cat $BOOTLOADER -binary -crop \
                        $BOOTLOADER_START_OFFSET $BOOTLOADER_END_OFFSET \
                $BOOTLOADER_CTX -binary -offset $BOOTLOADER_CTX_START_OFFSET -crop \
                        $BOOTLOADER_CTX_START_OFFSET $BOOTLOADER_CTX_END_OFFSET\
                $FIRMWARE -binary -offset $IMAGE_START_OFFSET -crop \
                        $IMAGE_START_OFFSET $IMAGE_END_OFFSET"
    echo $cmd
    $cmd -o firmware.hex -intel
    $cmd -o firmware.bin -binary
    echo "Adding the bootloader to the firmware...done"
    echo "Intel format created: firmware.hex"
    echo "Binary format created: firmware.bin"
}

check_args
check_firmware_tool
generate_manifest
generate_ota_image
generate_flashable_firmware
