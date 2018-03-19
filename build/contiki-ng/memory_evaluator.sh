#!/bin/bash
# This script is used to evaluate the size of the various configurations
set -e

# Evaluate Bootloader size
evaluate_bootloader() {
    echo "**Bootloader built with SEC_LIB=$1 VERBOSITY=$2 **"
    (cd bootloader && make clean && make -j2 SEC_LIB=$1 LOGGER_VERBOSITY=$2 EVALUATE_TIME=0) > /dev/null 2>&1
    size bootloader/bootloader.elf
}

evaluate_application() {
    echo "**Application built with SEC_LIB=$1 CONN_TYPE=$2 VERBOSITY=$3 **"
    (make clean && make -j2 SEC_LIB=$1 CONN_TYPE=$2 LOGGER_VERBOSITY=$3 EVALUATE_TIME=0) > /dev/null 2>&1
    size runner.elf
}

echo "Start evalutuation"
evaluate_bootloader TINYDTLS 0
evaluate_bootloader TINYCRYPT 0
evaluate_bootloader CRYPTOAUTHLIB 0

evaluate_application TINYDTLS CONN_UDP 0
evaluate_application TINYCRYPT CONN_UDP 0
evaluate_application CRYPTOAUTHLIB CONN_UDP 0

evaluate_application TINYDTLS CONN_DTLS_PSK 0
evaluate_application TINYDTLS CONN_DTLS_ECDSA 0

evaluate_application CRYPTOAUTHLIB CONN_DTLS_PSK 0
evaluate_application CRYPTOAUTHLIB CONN_DTLS_ECDSA 0

echo "Evaluation done"
