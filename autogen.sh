#!/bin/sh -e

# Fetch external dependencies
if [ -d "ext" ]; then
    rm -rf ext
fi
mkdir ext

clone() {
    echo "Cloning $1..."
    git clone --quiet --progress --single-branch --recursive --depth 2 \
        $2 ext/$1
    echo "Cloning $1...done"
}

# Clone the repositories
clone libcoap https://github.com/obgm/libcoap.git
clone tinydtls https://git.eclipse.org/r/tinydtls/org.eclipse.tinydtls
clone tinycrypt https://github.com/01org/tinycrypt.git

# Patch the repositories
PATCHDIR=patches
for dir in $(cd $PATCHDIR && find * -type d -print); do
    for f in $(find $PATCHDIR/$dir -maxdepth 1 -name *.patch); do
        patch=$PWD/$f
        echo "Applying patch: $patch"
        (cd ext/$dir/
        git am $patch
        )
    done
done
# Clone and build the libcoap library
echo "Build libcoap..."
(
  cd ext/libcoap && \
  ./autogen.sh && \
  ./configure --with-tinydtls --disable-shared \
  --disable-documentation --disable-examples && \
  make
)
echo "Build libcoap...done"

# Clone and build tinydtls
echo "Build tinydtls..."
(
  cd ext/tinydtls
  # Avoid error of configure (this file is not needed in the way we use tinydtls)
  touch install.sh
  POSIX_OPTIMIZATIONS="-Os -ffunction-sections -fdata-sections -Wl,--gc-sections"
  autoreconf -i --force
  ./configure --without-debug
  make libtinydtls.a CFLAGS="$POSIX_OPTIMIZATIONS"
)
echo "Build tinydtls...done"

#Build tinycrypt
(cd ext/tinycrypt && make)

# Check the presence of the testing tool
echo "Checking for ceedling test suite..."
command -v ceedling >/dev/null 2>&1 || \
    (echo "You should install the testing suite \
    `ceedling`.  Install it by running `gem install ceedling`"; exit 1);
echo "Checking for ceedling test suite...done"

# Building the firmware tool
echo "Building the firmware tool..."
(cd utils/firmware_tool && make)
echo "Building the firmware tool...done"

# Create the assets for the test
echo "Generating testing assets..."
if [ -d "assets" ]; then
    rm -rf assets/*
fi
./utils/assets_generator.sh
echo "Generating testing assets...done"
