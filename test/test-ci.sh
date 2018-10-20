#!/bin/bash -e

make
make firmware_tool
make server
make assets
./utils/server/server -f assets/slot_d.bin >/dev/null 2>&1 &
make clean # This is needed since the library has been build before without coverage support
make check

coveralls -e test -e ext -e /usr -e utils -e build -i src/core -i src/agents -b test -r . --gcov-options '\-lp' \

./test/push-size.sh
