#!/bin/sh -e

# This script should be called in the continuous integration service
# to make sure that the library still builds for all the supported platforms
# When using Zephyr we currently support the following platforms:
platforms="nrf52840_pca10056"

function build() {
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

# Prepare for the Zephyr build
./autogen.sh
source ext/zephyr/zephyr-env.sh

# nrf52840
for p in "$platforms"; do
    build $p "application"
    build $p "bootloader"
done
