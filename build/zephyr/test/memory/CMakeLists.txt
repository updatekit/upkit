include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(test_memory)

target_sources(app PRIVATE src/main.c)

set(ROOTDIR "../../../..")

# Add libpull to the build
file(GLOB libpull_src ${ROOTDIR}/src/**/*.c)
file(GLOB libpull_headers ${ROOTDIR}/include)
target_sources(app PRIVATE ${libpull_src})
include_directories(${libpull_headers})

add_definitions(-DSIMPLE_MANIFEST=1 -DLOGGER_VERBOSITY=3)
add_definitions(-DMEMORY_FLASH_IMPL=1 -DDISABLE_LIBPULL_NETWORK=1)

# Add the board files
set(BOARDDIR "../../board/${BOARD}")
include_directories(${BOARDDIR})
include_directories(${ROOTDIR}/test/platform)
file(GLOB board_src ${BOARDDIR}/*.c)
target_sources(app PRIVATE ${board_src})


# Add the Unity testing framework
file(GLOB unity_src ${ROOTDIR}/ext/unity/src/*.c)
file(GLOB unity_header ${ROOTDIR}/ext/unity/src/*.h)
target_sources(app PRIVATE ${unity_src})
include_directories(${ROOTDIR}/ext/unity/src)

