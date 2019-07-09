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
while [ $max -gt 0 ]; do
    echo "memory_test" >> $test_memory_file
    (( max-- ))
done
echo "Generate Test Memory file...done"

# Generate slots
echo "Generate slots..."
APPID=0x0202
slot_prefix="$ASSETSDIR/slot_"
versions="a b c d"
for v in $versions; do
    slot="$slot_prefix$v"
    # Creating slot of size 0x14000
    yes $v | awk '{ printf("%s", $0)}' | dd bs=1024 count=100 > $slot.tmp
    $FIRMWAREDIR/firmware_tool manifest generate -y $FIRMWAREDIR/config.toml -vv \
        -p $FIRMWAREDIR/keys/vendor.priv -c $FIRMWAREDIR/keys/vendor.pub \
        -k $FIRMWAREDIR/keys/server.priv -m $FIRMWAREDIR/keys/server.pub \
        -l $v -a $APPID -b $slot.tmp \
        -f $ASSETSDIR/metadata metadata
    srec_cat $ASSETSDIR/metadata -binary $slot.tmp -binary -offset 0x100 -o $slot.bin -binary
    cp $slot.bin $slot.pristine
    #rm $slot.tmp
done
echo "Generate slots...done"

# Generate the bootloader context
$FIRMWAREDIR/firmware_tool bootctx generate -f $ASSETSDIR/bootctx
$FIRMWAREDIR/firmware_tool bootctx generate -f $ASSETSDIR/bootctx.pristine

# Generate the LZSS compressed file
# The compressed file is a compressed version of slot_a.pristine
$FIRMWAREDIR/firmware_tool pipeline compress -b $ASSETSDIR/slot_a.pristine \
    -f $ASSETSDIR/slot_a.compressed

# Generate the bsdiff patch
# The patch is generated between slot_a.pristine and slot_b.pristine
$FIRMWAREDIR/firmware_tool pipeline diff -b $ASSETSDIR/slot_a.pristine \
    -z $ASSETSDIR/slot_b.pristine -f $ASSETSDIR/slot.patch

# Generate an empty file to be used as swap partition
yes | dd  bs=1024  count=4 > $ASSETSDIR/swap.bin

# Generate patch firmware from b to d
$FIRMWAREDIR/firmware_tool manifest generate -y $FIRMWAREDIR/config.toml -vv \
    -p $FIRMWAREDIR/keys/vendor.priv -c $FIRMWAREDIR/keys/vendor.pub \
    -k $FIRMWAREDIR/keys/server.priv -m $FIRMWAREDIR/keys/server.pub \
    -l "d" -a $APPID -b $ASSETSDIR/slot_d.tmp \
    -f $ASSETSDIR/slot_d.manifest
# Generate diff
$FIRMWAREDIR/firmware_tool pipeline diff -b $ASSETSDIR/slot_b.tmp \
    -z $ASSETSDIR/slot_d.tmp -f $ASSETSDIR/slot_d.patch.tmp
# Compress diff
$FIRMWAREDIR/firmware_tool pipeline compress -b $ASSETSDIR/slot_d.patch.tmp \
    -f $ASSETSDIR/slot_d.patch.diff.tmp
# Add manifest to patch to generate slot_d.patch which contains the patch and
# the manifest and is ready to be sent to a device
srec_cat $ASSETSDIR/slot_d.manifest -binary $ASSETSDIR/slot_d.patch.diff.tmp -binary\
    -offset 0x100 -o $ASSETSDIR/slot_d.patch -binary

echo "done"

