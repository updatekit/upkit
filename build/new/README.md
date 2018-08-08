#New Platform

If you want to integrate a new platform you will find
here a small tutorial suggesting the steps to do it
in the right way.

## Build libpull with the same build system

You have two approaches to build libpull:

- build a static library and link it to your platform;
- build it with your platform build system;

Both approaches should lead you to the same result,
however, the second one is more easier and less
error prone, thus is the one that we used for all
our supported platforms and is what we suggest also
in your case.

If you want to build libpull togeather with your build
system you may want to follow the approach already used
for the GNU Makefile, watching the Contiki-NG or RIOT
platforms, or the CMake used when building Zephyr.

## Build the tests

We provide three tests that can be easily executed on
the board. Each of them targets a specific feature
of libpull that is platform dependent.

* [memory test](https://github.com/libpull/libpull/blob/master/test/platform/memory.h): allows to verify the correct implementation of the memory interface;
* [security test](https://github.com/libpull/libpull/blob/master/test/platform/security.h): allows to test the security functions (such as digest ECC key validation) directly on hardware;
* [network test](https://github.com/libpull/libpull/blob/master/test/platform/network.h): allows to test the correct implementation of the connection interface.

Only when all these tests passes you can be sure that
libpull will work correctly with your platform and
that you implemented all the interfaces correctly.

To build the tests you can take as example the already
supported platforms.

## Build the application

The application you will execute on the IoT device must
periodically execute the [update agent](basic/agents.html)
to check if a new update is available and start in that
case the process.

Our update agent is build on top of coroutines. This means
that you can easily use it also with platforms where abstractions
such a thead or processes are not available and you only rely
on a main *while loop*.

You can follow the examples on the other supported platforms
to see how to invoke and configure the update agent. You will
find informations on the [API documentation](api/api.html).

## Build the bootloader

Our bootloader can be easily integrated with the OS to
benefit from all the facilities it offers. This allows, for example,
to support all the boards supported by the OS just by providing
support for it.

If you are building a bootloader for a specific solution and you
do not want to rely on an OS as a basis for it, you can build it
bare metal, still using the bootloader agent that we provide.

To see how to configure and execute the bootloader please check
the [API documentation](api/api.html).

## Contributing?

Do you think the new OS you are supporting may be useful
also for others? Please [open an issue](https://github.com/libpull/libpull/issues/new)
or a pull request to discuss a possible integration.
