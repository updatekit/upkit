cmake_minimum_required(VERSION 3.11)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(test_network)

target_sources(app PRIVATE src/main.c)

set(ROOTDIR "../../../..")

# Add libpull to the build
file(GLOB libpull_src ${ROOTDIR}/src/**/**/*.c)
file(GLOB libpull_headers ${ROOTDIR}/include)
target_sources(app PRIVATE ${libpull_src})
include_directories(${libpull_headers} ${ROOTDIR})

set(BOARDDIR ${ROOTDIR}/src/platform/nrf52840)
file(GLOB BOARDSRC ${BOARDDIR}/*.c)
target_sources(app PRIVATE ${BOARDSRC})
include_directories(${BOARDDIR})

set(CONNDIR ${ROOTDIR}/src/network/coap/zoap)
file(GLOB CONNSRC ${CONNDIR}/*.c)
target_sources(app PRIVATE ${CONNSRC})
include_directories(${CONNDIR})

add_definitions(-DSIMPLE_MANIFEST=1 -DLOGGER_VERBOSITY=3)
add_definitions(-DWITH_CONNECTION_ZOAP=1)
add_definitions(-DMEMORY_FLASH_IMPL=1 -DWITH_PLATFORM_NRF52840=1)

# Add the board files
include_directories(${ROOTDIR}/test/platform)
