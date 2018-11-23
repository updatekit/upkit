#!/bin/sh -e

clone() {
    echo "Cloning $1..."
    git clone --quiet --progress --recursive \
        $2 ext/$1
    if [ x"$3" != x"" ]; then
        (cd ext/$1; git reset --hard $3)
    fi
    echo "Cloning $1...done"
}

patch() {
    for dir in $(cd patches && find $1 -type d -print); do
        for f in $(find "patches/$dir" -maxdepth 1 -name "*.patch"| sort); do
            patch=$PWD/$f
            echo "applying patch $patch"
            (cd ext/$dir && git am --ignore-whitespace --ignore-space-change $patch)
        done
    done
}

if [ ! -d "ext" ]; then
    mkdir ext
fi

if [ ! -d "ext/libcoap" ]; then
    clone libcoap https://github.com/obgm/libcoap.git "16685d7"
    patch libcoap
    # Build libcoap
    (echo "Build libcoap..."
    cd ext/libcoap
    ./autogen.sh
    touch ext/tinydtls/install.sh
    ./configure --with-tinydtls --disable-shared --disable-examples
    make
    echo "Build libcoap...done")
fi
if [ ! -d "ext/tinydtls" ]; then
    clone tinydtls https://git.eclipse.org/r/tinydtls/org.eclipse.tinydtls
    patch tinydtls
    # Build tinydtls
    (echo "Build tinydtls..."
    cd ext/tinydtls
    touch install.sh
    POSIX_OPTIMIZATIONS="-Os -ffunction-sections -fdata-sections -Wl,--gc-sections"
    autoreconf -i --force
    ./configure --without-debug
    make libtinydtls.a CFLAGS="$POSIX_OPTIMIZATIONS"
    echo "Build tinydtls...done")
fi
if [ ! -d "ext/tinycrypt" ]; then
    clone tinycrypt https://github.com/01org/tinycrypt.git
    patch tinycrypt
    # Build tinycrypt
    echo "Build tinycrypt..."
    (cd ext/tinycrypt && make ENABLE_TESTS=false)
    echo "Build tinycrypt...done"
fi
if [ ! -d "ext/mbedtls" ]; then
    clone mbedtls https://github.com/ARMmbed/mbedtls.git
    # Build mbedtls
    echo "Build mbedtls..."
    (cd ext/mbedtls && make)
    echo "Build mbedtls...done"
fi
if [ ! -d "ext/cryptoauthlib" ]; then
    clone cryptoauthlib https://github.com/libpull/cryptoauthlib-hal.git
fi

autoreconf -i
