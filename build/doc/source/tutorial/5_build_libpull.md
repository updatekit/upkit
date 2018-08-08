# Libpull build
In this part of the tutorial you will learn how to build the libpull bootloader and update agent, build them togeather and flash them to the device.

## Zephyr build

To build the bootloader we need to go to the specific platform build folder. Libpull defines a folder for each platform containing the all the scripts and tools to work with the specific platform, in our case Zephyr.

```
$ cd ~/libpull_tutorial/libpull/build/zephyr
```

On this folder you will find the following content:

- **autogen.sh**: script to clone the dependencies; 
- **application**: folder containing the code and the build system for the application;
- **bootloader**: folder containing the code and build system for the bootloader;
- **board**: folder containing the boards specific files;
- **bootloader_ctx**: folder containing the bootloader context;
- **config.toml**: file indicating the options for the firmware_tool utility;
- **make_firmware.sh**: a script to build together the bootloader and the application in a single flashable image;
- **test**: a folder containing a set of tests that must pass on the board before executing libpull;

We will start executing the tests to see if they pass on the device and start undestanding how the build process works.

Before executing any compilation you should execute the:

```
$ ./autogen.sh
```
script contained in the `build/zephyr` folder that will download all the needed dependencies.

Once you have downloaded it you need to import the `zephyr-env.sh` variables to your environment. You can do it with the commands:

```
$ cd ext/zephyr
$ source zephyr-env.sh
```

## Execute libpull platform specific tests

Moving to the `test` folder we will find three tests with different goals:

- memory: tests the ability to read and write data to the flash;
- network: tests the ability to send and receive packets using our connection;
- security: tests the ability to perform the firmware verification using a specific cryptographic library;

This tests ensure that libpull correctly works on the choosen platform.

For each test you can build it entering in the test folder and typing the following commands:

```
$ mkdir build && cd build
$ cmake -GNinja -DBOARD=nrf52840_pca10056 -DCONF_FILE=prj.conf ..
$ ninja
$ ninja flash
$ minicom -D /dev/tty.your_device
```

***
⚠️ Remember to always import the Zephyr variables to your bash environment using the zephyr-env.sh
***

Analyzing the output of the test you can understand if it works on your device.

## Build the bootloader

To build the bootloader you need to perform the same Zephyr build steps in the bootloder directory:

```
$ cd ~/libpull_tutorial/libpull/build/zephyr/bootloader
$ mkdir build && cd build
$ cmake -GNinja -DBOARD=nrf52840_pca10056 -DCONF_FILE=prj.conf ..
$ ninja
```

The bootloader once loaded on the device needs to have a storage to save persistent data. This is what is in libpull is called the *bootloader_ctx*.
To create a bootloader context we provide a program contained in the `bootloader_ctx` folder of each platform.

You can build it using the following commands:

```
$ cd bootloader_ctx
$ make
```

If the build was successfull you can now move to the next step.

## Build the application

The application contains the update agent, in charge of contacting the server to receive the update. This means that the update agent must be able to commicate with the server and must know its IP address.

The IP address of the server, that in this case is the one of your *wpan0* network, must be hardcoded in the `application/src/runner.c` file, editing the `SERVER ADDR` preprocessor variable.

```
#define SERVER_ADDR <your_public_ipv6_address>
```

The public IPv6 address can be taken from the *ifconfig* command as previously shown in this tutorial.

To build the application you need to perform the same Zephyr build steps in the application directory:

```
$ cd ~/libpull_tutorial/libpull/build/zephyr/application
$ mkdir build && cd build
$ cmake -GNinja -DBOARD=nrf52840_pca10056 -DCONF_FILE=prj.conf ..
$ ninja
```

If the build was successfull you can now move to the next step.

## Create the flashable firmware

Now that we have a bootloader and an application we can build a flashable firmware. To do it each platform folder contains a `./make_firmware.sh` script that will invoke all the programs to create a flashable binary according to the specific variables of the board.

In the case of the *nRF52840* you can find the variables in the `board/nrf52840_pca10056/Makefile.conf` file.

To create a firmware you can invoke the previosly described script:

```
$ ./make_firmware.sh
```

If the firmware has been successfully created you will find it on the main folder in two formats:

- fimrware.hex (Intel Hex version)
- firmare.bin (Binary version)

## Flash the firmware

We can use now the `nrfjprog` to flash the firmware, using the command:

```
$ nrfjprog --eraseall
$ nrfjprog --program firmware.hex
$ nrfjprog --reset
```
Using minicom you can analyze the output. If everything is working correctly you should see the bootloader output:

```
***** Booting Zephyr OS v0.0.1-179-g67dce7f *****
Bootloader started
id: bootable 2 - non bootable 3
id: bootable 2 - non bootable 3
version: bootable 1 - non bootable 0
Start phase GET_OBJECT_MANIFEST
Start phase CALCULATING_DIGEST
Digest: initial offset 256u final offset 226396 size 226140
Start phase VERIFY_DIGEST
Start phase VERIFY_VENDOR_SIGNATURE
Vendor Signature Valid
Start phase VERIFY_SERVER_SIGNATURE
Server Signature Valid
loading object
loading address 9100
```

At the address 9100 the `./make_firmware.sh` script placed the application that should boot with the following output:

```
***** Booting Zephyr OS v0.0.1-179-g67dce7f *****
Zephyr Shell, Zephyr version: 1.12.0
Type 'help' for a list of available commands
shell> [net] [INF] ot_state_changed_handler: State changed! Flags: 0x000010e4 Current role: 2
[net] [INF] ot_state_changed_handler: State changed! Flags: 0x00000001 Current role: 2
[net] [INF] ot_state_changed_handler: State changed! Flags: 0x00000200 Current role: 2
[net] [INF] ot_state_changed_handler: State changed! Flags: 0x00000001 Current role: 2
Getting the newest firmware from memory
the id of the newest firmware is 2
Checking for updates
....
```

At this point we are ready for the last phase, starting the update server and sending a new image to the device.