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

# Clone TinyDTLS
echo "Cloning tinydtls..."
git clone --quiet --progress --recurse-submodules \
    --single-branch --depth 2 https://git.eclipse.org/r/tinydtls/org.eclipse.tinydtls \
    ext/tinydtls
echo "Cloning tinydtls...done"

# Clone CryptoAuthlib
echo "Cloning cryptoauthlib..."
git clone --quiet --progress --recursive \
    git@github.com:libpull/cryptoauthlib-hal.git \
    ext/cryptoauthlib
echo "Cloning cryptoauthlib...done"

# Clone tinycrypt
echo "Cloning tinycrypt..."
git clone --quiet --progress \
    --single-branch --depth 2 https://github.com/01org/tinycrypt.git \
    ext/tinycrypt
echo "Cloning tinycrypt...done"

# Patch the repositories
PATCHDIR=patches
for dir in $(cd $PATCHDIR && find * -type d -print); do
    for f in $(find $PATCHDIR/$dir -maxdepth 1 -name '*.patch' -print | sort); do
        patch=$PWD/$f
        echo "Applying patch: $patch"
        (cd ext/$dir/
        git am --ignore-whitespace $patch
        )
    done
done
