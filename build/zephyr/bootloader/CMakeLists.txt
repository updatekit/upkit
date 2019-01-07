cmake_minimum_required(VERSION 3.11)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(bootloader)

target_sources(app PRIVATE src/bootloader.c)

set(ROOTDIR "../../..")

# Add libpull to the build
file(GLOB_RECURSE libpull_src ${ROOTDIR}/src/*.c)
file(GLOB libpull_headers ${ROOTDIR}/include)
target_sources(app PRIVATE ${libpull_src})
include_directories(${libpull_headers} ${ROOTDIR})

add_definitions(-DSIMPLE_MANIFEST=1 -DLOGGER_VERBOSITY=0)
add_definitions(-DMEMORY_FLASH_IMPL=1 -DDISABLE_LIBPULL_NETWORK=1)
add_definitions(-DWITH_BOOTLOADER=1)
add_definitions(-DWITH_PLATFORM_NRF52840=1 -DWITH_ARM_CORTEX_LOADER=1)

set(TINYCRYPT 1) # TINYCRYPT or TINYDTLS or MBEDTLS
if (TINYCRYPT)
    add_definitions(-DWITH_TINYCRYPT=1)
elseif(TINYDTLS)
    add_definitions(-DWITH_TINYDTLS=1)
    include_directories(${ROOTDIR}/ext/tinydtls)
    file(GLOB ecc_src ${ROOTDIR}/ext/tinydtls/ecc/ecc.c)
    file(GLOB sha2_src ${ROOTDIR}/ext/tinydtls/sha2/sha2.c)
    add_definitions(-DWITH_SHA256 -DSHA2_USE_INTTYPES_H)
    target_sources(app PRIVATE ${ecc_src} ${sha2_src})
elseif(MBEDTLS)
    add_definitions(-DWITH_MBEDTLS=1)
endif(TINYCRYPT)

# Add the board headers
include_directories(${ROOTDIR}/src/platform/nrf52840)
