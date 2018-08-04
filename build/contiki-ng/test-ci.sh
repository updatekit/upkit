#!/bin/bash -e

ROOTDIR=$(cd $(dirname $0) && pwd -P)
# This script should be called in the continuous integration service
# to make sure that the library still builds for all the supported platforms
# When using RIOT we currently support the following platforms
platforms="remote-reva"

build() {
    (
    cd "$2"
    ls
    make && echo "$2 $1: Build successfull"
    )
}

(
    cd $ROOTDIR
    # Prepare for the build
    ./autogen.sh

    echo "Starting build"
    for p in $platforms; do
        build $p "application"
        build $p "bootloader"
    done
)
