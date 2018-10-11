include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(bootloader)

target_sources(app PRIVATE src/bootloader.c)

set(ROOTDIR "../../..")

# Add libpull to the build
file(GLOB_RECURSE libpull_src ${ROOTDIR}/src/*.c)
file(GLOB libpull_headers ${ROOTDIR}/include)
target_sources(app PRIVATE ${libpull_src})
include_directories(${libpull_headers} ${ROOTDIR})

add_definitions(-DSIMPLE_MANIFEST=1 -DLOGGER_VERBOSITY=1)
add_definitions(-DMEMORY_FLASH_IMPL=1 -DDISABLE_LIBPULL_NETWORK=1)
add_definitions(-DWITH_TINYCRYPT=1 -DWITH_BOOTLOADER=1)
add_definitions(-DWITH_PLATFORM_NRF52840=1 -DWITH_ARM_CORTEX_LOADER=1)

# Add the board headers
include_directories(${ROOTDIR}/src/platform/nrf52840)
