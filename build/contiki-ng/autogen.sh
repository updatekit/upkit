#!/bin/sh -e

# Fetch external dependencies
if [ -d "ext" ]; then
    rm -rf ext
fi
mkdir ext

# Clone the contiki repository and its submodules
echo "Cloning the Contiki-NG repository..."
git clone --quiet --progress \
    https://github.com/contiki-ng/contiki-ng.git ext/contiki-ng
(cd ext/contiki-ng
git checkout  d889b8f364
git submodule update --init --recursive
)
echo "Cloning the Contiki-NG repository...done"

# Clone cryptoauhlib
echo "Cloning the cryptoauthlib repository..."
git clone --quiet --progress \
    https://github.com/MicrochipTech/cryptoauthlib.git cryptoauthlib
echo "Cloning the cryptoauthlib repository...done"

PATCHDIR=patches
for dir in $(cd $PATCHDIR && find . -type d -print); do
    for f in $(find "$PATCHDIR/$dir" -maxdepth 1 -name "*.patch"| sort); do
        patch=$PWD/$f
        echo "Applying patch: $patch"
        (cd ext/$dir && git am --ignore-whitespace $patch)
    done
done
