include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(test_memory)

file(GLOB app_src src/*.c)
target_sources(app PRIVATE ${app_src})

set(ROOTDIR "../../..")

# Add libpull to the build
file(GLOB libpull_src ${ROOTDIR}/src/**/*.c)
file(GLOB libpull_headers ${ROOTDIR}/include)
target_sources(app PRIVATE ${libpull_src})
include_directories(${libpull_headers} ${ROOTDIR})

add_definitions(-DSIMPLE_MANIFEST=1 -DLOGGER_VERBOSITY=4)
add_definitions(-DMEMORY_FLASH_IMPL=1 -DWITH_TINYCRYPT=1)
add_definitions(-DDISABLE_LIBPULL_NETWORK=1 -DBUFFERED_WRITER=1)

# Add the board files
set(BOARDDIR "../board/${BOARD}")
include_directories(${BOARDDIR})
include_directories(${ROOTDIR}/test/platform)
file(GLOB board_src ${BOARDDIR}/*.c)
target_sources(app PRIVATE ${board_src})
