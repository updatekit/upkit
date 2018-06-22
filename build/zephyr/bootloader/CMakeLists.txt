include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(test_memory)

target_sources(app PRIVATE src/bootloader.c)

set(ROOTDIR "../../..")

# Add libpull to the build
file(GLOB libpull_src ${ROOTDIR}/src/**/*.c)
file(GLOB libpull_headers ${ROOTDIR}/include)
target_sources(app PRIVATE ${libpull_src})
include_directories(${libpull_headers} ${ROOTDIR})

add_definitions(-DSIMPLE_MANIFEST=1 -DLOGGER_VERBOSITY=4)
add_definitions(-DMEMORY_FLASH_IMPL=1 -DDISABLE_LIBPULL_NETWORK=1)
add_definitions(-DWITH_TINYCRYPT=1)

# Add the board files
set(BOARDDIR "../board/${BOARD}")
include_directories(${BOARDDIR})
file(GLOB board_src ${BOARDDIR}/*.c)
target_sources(app PRIVATE ${board_src})

# Add the bootloader agent source
file(GLOB bootloader_src ${ROOTDIR}/agents/bootloader_agent.c)
target_sources(app PRIVATE ${bootloader_src})

