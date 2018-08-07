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

If the build was successfull you can now move to the next step.

## Build the application

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

If the firmware has been build successfully you will find it 



