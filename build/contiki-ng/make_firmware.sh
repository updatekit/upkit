#!/bin/sh
# This script creates an ota image, containing the bootloader,
# the metadata and the binary image
set -e

ROOTDIR=$(cd $(dirname $0) && pwd -P)

# Default paths
FIRMWARE_TOOL_DIR="$ROOTDIR/../../utils/firmware_tool/"
FIRMWARE_TOOL="$FIRMWARE_TOOL_DIR/firmware_tools"
FIRMWARE_DIR="$ROOTDIR/firmware"

# Configurations
VERSION="0001"
APPLICATION="1111"
BOOTLOADER="$ROOTDIR/bootloader/bootloader.hex"
BOOTLOADER_CTX="$ROOTDIR/bootloader/bootloader_ctx.bin"
METADATA="$FIRMWARE_DIR/metadata.bin"
IMAGE="$ROOTDIR/runner.bin"
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
        echo "Bootloader ctx not present. Exiting...\n"
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

    cp $FIRMWARE_TOOL $FIRMWARE_DIR/firmware_tools
    FIRMWARE_TOOL=$FIRMWARE_DIR/firmware_tools
}

check_keys() {
    if [[ ! -f $FIRMWARE_DIR/prov_key.priv || ! -f $FIRMWARE_DIR/prov_key.pub ]]; then
        echo "Copying default provisioning keys..."
        cp $FIRMWARE_TOOL_DIR/prov_key.priv $FIRMWARE_DIR/prov_key.priv
        cp $FIRMWARE_TOOL_DIR/prov_key.pub $FIRMWARE_DIR/prov_key.pub
        echo "Copying default provisioning keys...done"
    fi
    if [[ ! -f $FIRMWARE_DIR/vend_key.priv || ! -f $FIRMWARE_DIR/vend_key.pub ]]; then
        echo "Copying default vendor keys..."
        cp $FIRMWARE_TOOL_DIR/vend_key.pub $FIRMWARE_DIR/vend_key.pub
        cp $FIRMWARE_TOOL_DIR/vend_key.priv $FIRMWARE_DIR/vend_key.priv
        echo "Copying default vendor keys...done"
    fi
}

generate_metadata() {
    echo "Generating firmware metadata..."
    (cd $FIRMWARE_DIR && $FIRMWARE_TOOL -vv -l $VERSION -a $APPLICATION -b $IMAGE metadata)
    echo "Generating firmware metadata...done"
}

generate_ota_image() {
    echo "Adding the metadata to the firmware..."
    srec_cat $METADATA -binary $IMAGE -binary -offset 0x100 -o $FIRMWARE -binary
    echo "Adding the metadata to the firmware...done"
}

generate_flashable_firmware() {
    echo "Adding the bootloader to the firmware..."
    cmd="srec_cat $BOOTLOADER -intel -crop 0x0 0x5000 0x1FFA8 0x20000 \
        $FIRMWARE -binary -offset 0x5000 -crop 0x5000 0x1F000 \
        $BOOTLOADER_CTX -binary -offset 0x1F000 -crop 0x1F000 0x1FFA8"
    $cmd -o firmware.hex -intel
    $cmd -o firmware.bin -binary
}

check_args
check_firmware_tool
check_keys
generate_metadata
generate_ota_image
generate_flashable_firmware
