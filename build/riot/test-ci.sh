#!/bin/sh -e

# This script should be called in the continuous integration service
# to make sure that the library still builds for all the supported platforms
# When using RIOT we currently support the following platforms
platforms="remote-reva"

build_application() {
    (
    cd application
    make prepare && \
    make && echo "application: $1: Build successfull"
    )
}

build_bootloader() {
    (
    cd bootloader
    make && echo "bootloader: $1: Build successfull"
    )
}

ROOTDIR=$(cd $(dirname $0) && pwd -P)
(
    cd $ROOTDIR

    # Prepare for the build
    ./autogen.sh

    for p in $platforms; do
        build_application $p
        build_bootloader $p
    done
)
