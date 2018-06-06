#!/bin/sh -e

# Fetch external dependencies
if [ -d "ext" ]; then
    rm -rf ext
fi
mkdir ext

clone() {
    echo "Cloning $1..."
    git clone --quiet --progress --recursive \
        $2 ext/$1
    if [ x"$3" != x"" ]; then
        (cd ext/$1; git reset --hard $3)
    fi
    echo "Cloning $1...done"
}

# Clone the repositories
clone libcoap https://github.com/obgm/libcoap.git "16685d7"
clone tinydtls https://git.eclipse.org/r/tinydtls/org.eclipse.tinydtls
clone tinycrypt https://github.com/01org/tinycrypt.git
clone unity https://github.com/ThrowTheSwitch/Unity.git
clone cryptoauthlib https://github.com/MicrochipTech/cryptoauthlib.git

# Patch the repositories
PATCHDIR=patches
for dir in $(cd $PATCHDIR && find * -type d -print); do
    for f in $(find $PATCHDIR/$dir -maxdepth 1 -name *.patch| sort); do
        patch=$PWD/$f
        echo "Applying patch: $patch"
        (cd ext/$dir && git am --ignore-whitespace $patch)
    done
done

# Build libcoap
(echo "Build libcoap..."
cd ext/libcoap
./autogen.sh
touch ext/tinydtls/install.sh
./configure --with-tinydtls --disable-shared --disable-examples
make
echo "Build libcoap...done")

# Build tinydtls
(echo "Build tinydtls..."
cd ext/tinydtls
touch install.sh
POSIX_OPTIMIZATIONS="-Os -ffunction-sections -fdata-sections -Wl,--gc-sections"
autoreconf -i --force
./configure --without-debug
make libtinydtls.a CFLAGS="$POSIX_OPTIMIZATIONS"
echo "Build tinydtls...done")

# Build tinycrypt
echo "Build tinycrypt..."
(cd ext/tinycrypt && make ENABLE_TESTS=false)
echo "Build tinycrypt...done"

autoreconf -i
