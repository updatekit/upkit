# Network Setup

To send the firmware Over The Air using the [OpenThread](https://openthread.io) network we need to setup an [OpenThread border router](https://openthread.io/guides/border-router).

***
ℹ️ The setup suggested by the [official OpenThread documentation](https://openthread.io/guides/border-router) requires to use a Raspberry Pi 3 or a BeagleBone Black. Since we want to keep the setup simple we will describe a border router configuration when the thread device is directly connected to a computer. However, if you already have such a setup or you prefer to follow the official guide skip the following sections.
***

## Install a Linux Virtual Machine

If you are using Mac OSX you need to install a virtual machine since the OTBR tool provided by
OpenThread works only on Linux.


## Download and flash the border router

To install a Thread border router you can follow two paths:

- clone the repository and build it;
- download an already built version;

Since [OpenThread provides an already built version](https://openthread.io/guides/ncp/firmware) for our board we will follow the second approach.

***
⚠️ We assume you have some knowledge on the Thread networks. If not, you might want to run the [OpenThread Simulation Codelab](https://codelabs.developers.google.com/codelabs/openthread-simulation/#0), to get familiar with the basics Thread concepts.
***

You can download a prebuild version of the firmware using the following commands:

```
$ wget https://openthread.io/guides/ncp/ot-ncp-ftd-gccb354fb-nrf52840.tar.gz
$ tar -xzvf ot-ncp-ftd-gccb354fb-nrf52840.tar.gz
```

Now you should have a hex file called `ot-ncp-ftd-gccb354fb-nrf52840.hex` containing the firmware. You can flash it following the steps at [this link](https://openthread.io/guides/ncp/firmware) or following the next steps:

```
$ nrfjprog -f nrf52 --chiperase --program ot-ncp-ftd-gccb354fb-nrf52840.hex --reset
```

If the flashing was successfull you should see the following output:

```
Parsing hex file.
Erasing user available code and UICR flash areas.
Applying system reset.
Checking that the area to write is not protected.
Programming device.
Applying system reset.
Run.
```

## Connect and test the border router

Since the flashed firmware enables the use of native USB CDC ACM as a serial transport, we need to connect the board using the other micro USB.

***
ℹ️ You can have a visual description at [this link](https://openthread.io/guides/ncp/firmware).
***

1. Power off the board;
2. Disconnect the micro USB from the board;
3. Change the `nRF power source` switch from VDD to USB;
4. Attach the micro USB cable to the nRF USB port on the long side of the board;
5. Power on the board;

If the previous passages have been performed correctly you should not see all leds off on the board.

