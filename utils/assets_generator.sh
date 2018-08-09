#!/bin/bash
set -e

# The goal of this script is to generate the assets for the
# testing framework. It needs to generate four slot with
# different versions: 0xa 0xb 0xc 0xd that will be used
# also as slots during the testing phase
# The testing framework will be able to restore the assets after
# each test, and for this reason each slot must be generated
# two times: one time as a slot_X.bin and one time as
# slot_X_pristine.bin

ROOTDIR=$(cd $(dirname $0)/.. && pwd -P)
UTILSDIR=$ROOTDIR/utils
ASSETSDIR=$ROOTDIR/assets
FIRMWAREDIR=$UTILSDIR/firmware_tool

# Create assets directory
if [ ! -d "$ASSETSDIR" ]; then
    mkdir "$ASSETSDIR"
fi

# Generate Test Memory file
echo "Generate Test Memory file..."
test_memory_file=$ASSETSDIR/test_memory
max=64
for i in $(seq 1 $max); do
    echo "memory_test" >> $test_memory_file
done
echo "Generate Test Memory file...done"

# Generate slots
echo "Generate slots..."
APPLICATION=0x1001
slot_prefix="$ASSETSDIR/slot_"
versions="a b c d"
for v in $versions; do
    slot="$slot_prefix$v"
    # Creating slot of size 0x14000
    yes $v | awk '{ printf("%s", $0)}' | dd bs=4096 count=20 > $slot.tmp
    $FIRMWAREDIR/firmware_tool manifest generate -y $FIRMWAREDIR/config.toml -vv \
        -p $FIRMWAREDIR/keys/vendor.priv -c $FIRMWAREDIR/keys/vendor.pub \
        -k $FIRMWAREDIR/keys/server.priv -m $FIRMWAREDIR/keys/server.pub \
        -l $v -a $APPLICATION -b $slot.tmp \
        -f $ASSETSDIR/metadata metadata
    srec_cat $ASSETSDIR/metadata -binary $slot.tmp -binary -offset 0x100 -o $slot.bin -binary
    cp $slot.bin $slot.pristine
    rm $slot.tmp
done
echo "Generate slots...done"

# Generate the bootloader context
$FIRMWAREDIR/firmware_tool bootctx generate -f $ASSETSDIR/bootctx
$FIRMWAREDIR/firmware_tool bootctx generate -f $ASSETSDIR/bootctx.pristine

# Generate the LZSS compressed file
# The compressed file is a compressed version of slot_a.pristine
$FIRMWAREDIR/firmware_tool pipeline compress -b $ASSETSDIR/slot_a.pristine \
    -f $ASSETSDIR/slot_a.compressed
