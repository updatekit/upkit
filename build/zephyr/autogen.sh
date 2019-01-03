#!/bin/sh -e

# Fetch external dependencies
if [ -d "ext" ]; then
    rm -rf ext
fi
mkdir ext

# Clone the RIOT repository and its submodules
echo "Cloning the Zephyr repository..."
git clone --quiet --progress --depth 1 -b master \
    https://github.com/zephyrproject-rtos/zephyr ext/zephyr
(cd ext/zephyr
git checkout "v1.12.0"
git submodule update --init --recursive
)
echo "Cloning the Zephyr repository...done"
