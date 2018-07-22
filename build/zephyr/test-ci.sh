#!/bin/bash -e
ROOTDIR=$(cd $(dirname $0) && pwd -P)

# These flags are required by the Zephyr buildchain
unset GCCARMEMB_TOOLCHAIN_PATH
export ZEPHYR_TOOLCHAIN_VARIANT=cross-compile
export CROSS_COMPILE=/usr/bin/arm-none-eabi-

# This script should be called in the continuous integration service
# to make sure that the library still builds for all the supported platforms
# When using Zephyr we currently support the following platforms:
platforms="nrf52840_pca10056"

build() {
    if [ ! -d "build" ]; then
        mkdir build
    fi
    if [ -d "build/$2" ]; then
        rm -rf "build/$2"
    fi
    mkdir "build/$2"
    (
    cd "build/$2"
    cmake -GUnix\ Makefiles -DBOARD=$1 -DCONF_FILE=prj_$1.conf \
        ../../$2
    make && echo "$2 $1: Build successfull"
    )
}

(
    cd $ROOTDIR
    # Prepare for the Zephyr build
    ./autogen.sh

    # Install python packages required by the build toolchain
    pip install -r ext/zephyr/scripts/requirements.txt

    source ext/zephyr/zephyr-env.sh
    for p in $platforms; do
        build $p "application"
        build $p "bootloader"
    done
)
