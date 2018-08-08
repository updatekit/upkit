# Getting Started

## Prepare the Toolchain

We assume you already downloaded and installed the [ARM toolchain](https://launchpad.net/gcc-arm-embedded).
You can test if it works typing:

    $ arm-none-eabi-gcc --version

## Clone the Zephyr repository

Create a folder in your home directory and move to it:

    $ mkdir ~/libpull_tutorial
    $ cd ~/libpull_tutorial

Clone the Zephyr repository:

    $ git clone https://github.com/zephyrproject-rtos/zephyr

## Build a Zephyr example

To test if your setup is ready to work with Zephyr and the nRF52840 board,
build the hello world sample provided by the Zephyr project and load it to
the board.

You can follow the [official documentation](http://docs.zephyrproject.org/boards/arm/nrf52840_pca10056/doc/nrf52840_pca10056.html) for this task or the next steps:

    $ cd zephyr
    $ source zephyr-env.sh
    $ cd samples/hello_world
    $ mkdir build && cd build
    $ cmake -GNinja -DBOARD=nrf52840_pca10056 ..
    $ ninja
    
If the build was successfull you are now ready to flash the firmware
on the device:

    $ ninja flash
    
To read the serial output we use [Minicom](https://en.wikipedia.org/wiki/Minicom), but you can use every serial
communication program you like (i.e., screen).

If everthing was correct you should see the following output:

    ***** Booting Zephyr OS v1.12.0-290-g7a7e4f583 *****
    Hello World! arm

## Install the flashing tool

To flash the libpull generated firmware we will use `nrfjprog`. You can have it
instaling the 
[nRF5x Command Line Tools](https://www.nordicsemi.com/eng/Products/nRF52840#Downloads).

This program is needed to interact with the nRF52840 board. To test if it works
use the command:

```
$ nrfjprog --ids
```

that shows the serial numbers of all the boards connected to the computer.

