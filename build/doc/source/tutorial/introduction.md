# Introduction

In this tutorial you will undestand the steps needed
to configure libpull and integrate it with your platform.

This tutorial is based the Zephyr platform and the Nordic nRF52840
board.

To complete this tutorial you will need the following
components:

- 2x [Nordic nRF52840](https://www.nordicsemi.com/eng/Products/nRF52840);
- 2x Micro USB Male to USB A Male cable;
- A computer with Linux or Mac OSX;
  (in the future we will provide another tutorial
  for Windows)
- The [Arm toolchain](https://launchpad.net/gcc-arm-embedded) already installed;

![board](https://www.nordicsemi.com/var/ezwebin_site/storage/images/news/ulp-wireless-update/a-brief-guide-to-bluetooth-5/2176564-1-eng-GB/A-brief-guide-to-Bluetooth-5_full_article.jpg)

To follow this guide we do not assume any specific IDE and we assume we are familiar
with the terminal.

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

You can follow the [official documentation](http://docs.zephyrproject.org/boards/arm/nrf52840_pca10056/doc/nrf52840_pca10056.html) for this task.

The are the steps you may follow to complete this task, however it really
depends on you configurations:

    $ cd zephyr
    $ source zephyr-env.sh


