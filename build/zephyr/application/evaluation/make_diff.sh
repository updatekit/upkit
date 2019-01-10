#!/bin/bash

ROOTDIR=$(cd $(dirname $0)/.. && pwd -P)
FWTOOL=$ROOTDIR/../../../utils/firmware_tool/firmware_tool


function make_diff {
    $FWTOOL pipeline diff -b $1 -z $2 -f $ 3

}

make_diff fwV1.0.bin fwV1.1.bin fwV1.1.final
make_diff fwV1.0.bin fwV1.2.bin fwV1.2.final
make_diff fwV1.0.bin fwV1.3.bin fwV1.3.final
make_diff fwV1.0.bin fwV1.4.bin fwV1.4.final

