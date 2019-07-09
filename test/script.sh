#!/bin/bash -e

# The slot D is passed by the server
# The slot B is used for getting the oldest version
# The slot A is used for storing the newest gernerated version

cp ../build/zephyr/application/evaluation/fwV1.0.bin.final \
    ../assets/slot_b.bin

cp ../build/zephyr/application/evaluation/fwV1.2.bin.delta  \
   ../assets/slot_d.patch
cp ../build/zephyr/application/evaluation/fwV1.2.bin.final  \
        ../assets/slot_d.bin

make test_update_pull && ./test_update_pull
