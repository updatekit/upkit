#!/bin/sh -e

ROOTDIR=$(cd $(dirname $0) && pwd -P)
(
    cd $ROOTDIR

    # This script should be called in the continuous integration service
    # to make sure that the library still builds for all the supported platforms
    # When using RIOT we currently support the following platforms
    platforms="remote-reva"

    function build() {
        (
        cd "$2"
        make && echo "$2 $1: Build successfull"
        )
    }

    # Prepare for the build
    ./autogen.sh

    for p in "$platforms"; do
        build $p "application"
        build $p "bootloader"
    done
)
