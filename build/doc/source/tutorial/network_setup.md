# Network Setup

To send the firmware Over The Air using the [OpenThread](https://openthread.io) network we need to setup an [OpenThread border router](https://openthread.io/guides/border-router).

***
ℹ️ The setup suggested by the [official OpenThread documentation](https://openthread.io/guides/border-router) requires to use a Raspberry Pi 3 or a BeagleBone Black. Since we want to keep the setup simple we will describe a border router configuration when the thread device is directly connected to a computer. However, if you already have such a setup or you prefer to follow the official guide skip the following sections.
***

## Install the required packages

```
$ sudo apt-get install -y software-properties-common
```

## Download and flash the border router

***
⚠️ We assume you have some knowledge on the Thread networks. If not, you might want to run the [OpenThread Simulation Codelab](https://codelabs.developers.google.com/codelabs/openthread-simulation/#0), to get familiar with the basics Thread concepts.
***

