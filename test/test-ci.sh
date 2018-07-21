#!/bin/bash -e

make
make firmware_tool
make server
make assets
./utils/server/server -f assets/slot_d.bin >/dev/null 2>&1 &
make clean # This is needed since the library has been build before without coverage support
make check
