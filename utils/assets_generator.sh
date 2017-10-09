#!/bin/sh
set -e

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
for i in `seq 1 $max`; do
    echo "memory_test" >> $test_memory_file
done
echo "Generate Test Memory file...done"

# Generate Firmwares
echo "Generate Firmwares..."
APPLICATION=0x1001
fake_firmware="$ASSETSDIR/fake_firmware_"
# The first is the gold image
versions="dead beef"
for v in $versions; do
    f="$fake_firmware$v"
    # Creating firmware of size 0x14000
    dd if=/dev/zero bs=4096 count=20 > $f.tmp
    $FIRMWAREDIR/firmware_tools -vv -l $v -a $APPLICATION -b $f.tmp \
    -p $FIRMWAREDIR/vend_key.priv -c $FIRMWAREDIR/vend_key.pub \
    -k $FIRMWAREDIR/prov_key.priv -m $FIRMWAREDIR/prov_key.pub \
    -f $ASSETSDIR/metadata metadata
    hash=$(shasum -a 256 $f.tmp | cut -f 1 -d ' ')
    echo $hash
     srec_cat $ASSETSDIR/metadata -binary $f.tmp -binary -offset 0x100 -o $f -binary
     #rm $f.tmp
done
echo "Generate Firmwares...done"
firmware_a=$ASSETSDIR/fake_firmware_beef # Already generated
firmware_b=$ASSETSDIR/fake_firmware_dead # Already generated
firmware_gold=$ASSETSDIR/fake_firmware_gold # To generate
cp $firmware_a $firmware_gold

# Generate External Flash files
echo "Generate External Flash file..."
external_flash_simulator=$ASSETSDIR/external_flash_simulator
srec_cat $firmware_a -binary -offset 0x19000 \
         $firmware_gold -binary -offset 0x4B000 \
         -o $external_flash_simulator -binary
cp $external_flash_simulator "$external_flash_simulator"_updated
echo "Generate External Flash file...done"

# Generate Internal Flash files
echo "Generate Internal Flash file..."
internal_flash_simulator=$ASSETSDIR/internal_flash_simulator
expected_internal_flash_simulator=$ASSETSDIR/expected_internal_flash_simulator
# 100 kB of user data + 100 kB of firmware_a + 100 kB of firmware_b
srec_cat  $firmware_a -binary -offset 0x7000 \
         -o $internal_flash_simulator -binary
srec_cat  $firmware_a -binary -offset 0x7000 \
         -o $expected_internal_flash_simulator -binary
# Create another internal flash that will be updated
cp $internal_flash_simulator "$internal_flash_simulator"_updated
echo "Generate Internal Flash file...done"
