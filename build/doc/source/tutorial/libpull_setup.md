# Libpull Setup

In this part of the tutorial we will see how to work with libpull, setup the server, compile and install the bootloader and the application on the device.

## Clone libpull

Moving back to the tutorial folder previously created we are now ready to clone the libpull repository.

```
$ cd ~/libpull_tutorial
$ git clone https://github.com/libpull/libpull
$ cd libpull
```

## Build the library

We are now ready to build the library. The build system is based on the GNU Build system and is currently compatible onlye with Linux and Mac OSX.

***
ℹ️ We are planning to move the build system to CMake.
***
    
To build the library we first need to download the dependencies:

```
$ ./autogen.sh
```
    
This script will donwload and compile the dependencies, and call the `autoreconf` program, in charge of generating the configure script.

We can now configure the library using:

```
$ ./configure
```

Too see the configuration options type `./configure --help`.

The previous script will generate the Makefile for the whole project. We can now build the library calling make in the home directory.

```
$ make
```

If the process is successfull we are ready to move forward.

## Build and execute the server

Since the update must be downloaded OTA (Over The Air) we need a server. Libpull currently provides a testing server.

***
⚠️ The server is not ready for production.
***

To execute the server you can use the makefile target `run_server`. It will automatically create the assets, build the server and execute it. Otherwise, to have a better undestanding on the process you can read the `Makefile` and execute:

```
$ make assets
$ make server
$ make run_server
```

The first command will invoke the script `utils/assets_generator.sh` that will create a new folder called `assets` and place inside it several files used by the unit tests.

## Execute the library tests

If you want to be sure that the library has been built correctly and the correct execution of the server you can execute the Unit Tests with the following command:

````
make check
````

If all the tests passes you configuration is correct. If not, you should check the output printed on the server since you may have some network configuration (i.e. firewall) that is blocking the packets to the server.

If everything works we are now ready to start testing libpull on the device.
