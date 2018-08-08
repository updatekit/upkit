#Sending the update

In this last step of the tutorial you will learn how to put all the pieces togeather and send the update to the device.

## Build the update

We first need to create an update with a version higher than the one currently running on the device.

The version of the update must be setted in the `config.toml` configuration file. It is used by the `firmware_tool` application provided with libpull.

The default firmware version is `0x0001`. To make the update possible we should send to the device a firmware with a version stricly higher than the one already installed. We can, for example, set the version to `0x0002`.

Once we have an higher version we can generate the firmware using the `./make_firmware.sh` script.

This will generate, as before, a firmware containing both the application with the manifest and the bootloader. However, to send an update we want to send only the application part, since the bootloader cannot be changed on the device. Thus we need to take it from the `firmware` folder where the application firmware is generated.

Inside of the `firmware` folder (located at `~/libpull_tutorial/libpull/build/zephyr/firmware/`) we can find the following files:

- firmware.bin.tmp // The firmware generated by the application build system;
- manifest.bin     // The manifest created from it;
- firmware.bin     // A binary composed by the manifest + the firmware;

The server must send only the last one, thus we should pass it as a server argument.

## Execute the server

The `make run_server` target used at the start of the tutorial prepares the server for the Unit Tests. However, we need to send the firware previsouly created, thus we can execute the server with the following commands:

```
$ cd ~/libpull_tutorial/libpull/
$ ./utils/server/server -f build/zephyr/firmware/firmware.bin
```

However, the previous command is not sufficient since, until we configured the internal network to route packets from *wpan0* to the localhost, we also need to instruct the server to listend on the correct interface.

The server offers an option to do it:

```
./utils/server/server -A <your_public_ipv6_addr> -f firmware/firmware.bin
```

Where *<your_public_ipv6_addr>* is a global IPv6 address assigned to your *wpan0* interface.

## Update the device

Once the server is started, you can restart the device and wait for the update process to start. It should start immediately, downloading the manifest and then the whole update.

If the process was correct, you should see an output like this:

```
***** Booting Zephyr OS v0.0.1-179-g67dce7f *****
Bootloader started
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
[net] [INF] openthread_init: OpenThread version: OPENTHREAD/20170716-00461-gdb4759cc-dirty; NONE; Aug  7 2018 16:43:32
[net] [INF] openthread_init: Network name:   Libpull Tutorial
[net] [INF] ot_state_changed_handler: State changed! Flags: 0x003f333f Current role: 1
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
subsriber_cb: message received
Latest_version:    1 Provisioning version    2
An update is available
An update is available
Pages erased correctly
Manifest still not received
Received 64 bytes. Expected 0 bytes
Manifest still not received
Received 128 bytes. Expected 0 bytes
Manifest still not received
Received 192 bytes. Expected 0 bytes
Manifest still not received
Received 256 bytes. Expected 0 bytes
Manifest still not received
Manifest received
Platform: beef
Version: 0002
Size: 226140
valid: 0101 0202 received 0101 0202
Received 320 bytes. Expected 226396 bytes
...
Received 226396 bytes. Expected 226396 bytes
Start phase GET_OBJECT_MANIFEST
Start phase CALCULATING_DIGEST
Digest: initial offset 256u final offset 226396 size 226140
Start phase VERIFY_DIGEST
Start phase VERIFY_VENDOR_SIGNATURE
Vendor Signature Valid
Start phase VERIFY_SERVER_SIGNATURE
Server Signature Valid
```


