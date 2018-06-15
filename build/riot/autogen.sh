#!/bin/sh -e

# Fetch external dependencies
if [ -d "ext" ]; then
    rm -rf ext
fi
mkdir ext

# Clone the RIOT repository and its submodules
echo "Cloning the RIOT repository..."
git clone --quiet --progress \
    https://github.com/RIOT-OS/RIOT ext/riot
(cd ext/riot
git checkout "2018.04"
git submodule update --init --recursive
)
echo "Cloning the RIOT repository...done"

PATCHDIR=patches
for dir in $(cd "$PATCHDIR" && find . -type d -print); do
    for f in $(find "$PATCHDIR/$dir" -maxdepth 1 -name "*.patch"| sort); do
        patch=$PWD/$f
        echo "Applying patch: $patch"
        (cd ext/$dir && git am --ignore-whitespace $patch)
    done
done
