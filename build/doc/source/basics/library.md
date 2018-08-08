#Library Logic

Libpull has been build around of three main properties:

- **security**;
- **portability**;
- **platform constraints**;

It targets Class 1 constrained devices, characterized by 100 kB of ROM and 10 kB or RAM, as defined in RF 7228, but thanks to its small memory footprint and modular approach can be useful even on more powerful devices.

## Documentation Terminology

To understand the following sections is necessary to agree on some terminology and concepts used to analyze and describe the update process.

![terminology_image](https://github.com/libpull/libpull_graphics/raw/master/documentation/terminology_image.png)

We first define the terminology to describe an update:

- *image*: the software that will be executed on the device. It can be a firmware, in case it must be loaded by the device MCU, or could be a software module that can be loaded at runtime by the OS;
- *manifest*: the set of data related to the image, describing its size, its version and including all the data used for cryptographic verification;
- *update image*: the union of image and manifest representing the update. This is the data that must be generated and transmitted by the server to the client.

We also define the terminology to describe how an update is stored in memory as:

- *memory object*: the section of memory used to store the update. This can be a file, a segment of the internal or external flash, a device or any other memory abstraction defined by the user of the library;
- *running object*: the currently running image;
- *device memory*: a generic memory of the device that contains one running image and one or more memory objects.

## Software Updates Components

Analyzing the structure of the update process we identified three distinct components:

* The **vendor server** is the server owned by the vendor. This server is the first point where the update is built and thus, it is used to assert its integrity and authenticity. This server perform the following actions:
    * Builds the image;
    * Generates a manifest file;
    * Generates the update image;
    * Sends the update image to the provisioning server.
    
* The **provisioning server** is the server in charge of communicating with the device. It may be managed by the device vendor or not. It performs the following actions:
    * Notifies updates availability;
    * Updates the manifest;
    * Sends the update image to the device;
    * Logs the device status.
    
* The **update agent** is the code running on the device with the goal of getting the newest firmware available. It performs the following actions:
    * Checks the presence of updates; 
    * Receives the updates;
    * Validates the update.

## Portability Requirements

Portability can be considered as the possibility to reuse the same software in many environments. This required to designing the solution with the appropriate abstraction layers that allow to configure it according to the device capabilities and required logic. We identified four portability requirements:

* **Operating System agnostic**: IoT devices are based on a multitude of operating systems (mainly RTOS). The libpull core has been designed as a freestanding core that does not uses any any OS-specific API but instead relies on abstraction layers that must be specialized for each OS.
* **Network protocol agnostic**: Libpull has been designed to support many network protocols. For example, we already tested it with CoAP/CoAPS and BLE, but with more powerful devices, it can easily support the HTTP/HTTPS.
* **Cryptographic library agnostic**: The library has been implemented to make sure it is compatible with a [multitude of cryptographic libraries](cryptographic_libraries.html). We currently tested them with TinyDTLS, tinycrypt and the Atmel CryptoAuthLib.
* **Manifest encoding agnostic**. The manifest describing the update can be encoded using different formats, such as simple binary format, JSON, CBOR, to support outcoming solutions and needs.

## Constrained Devices Requirements

The library has a small memory footprint since it must be suitable for Class 1 devices. We are continuously monitoring this aspect using a script executed in the CI build, to make sure we know how each modification impacts the library size.

Moreover, the library supports many memory types. We tested it with standard Linux files but also with direct flash access. Moreover, the memory slots are configurable in a way that they can be placed in different memories, supporting also IoT devices with an external flash connected to them.

We also aims to cover several update methods, namely:
static, dynamic and seamless software updates.

* **Dynamic Software Updates**. In this configuration, the update image is represented by a module that can be loaded at runtime by the running image. The advantage of this configuration is that no-reboot is needed, making it suitable also for real-time application with high availability needs. To allow the use of this configuration, the OS must be capable of loading, and if necessary linking, the modules at runtime. The library does not explicitly manage the activation and relinking of the code, since it is a process highly bounded with the platform choises.
* **Static Software Updates**. In this configuration, the update image is represented by the whole OS. In this method, the presence of a bootloader is required. The advantage of this configuration is the possibility to perform atomical updates, loading a new image, and avoiding the problems of dynamic linking. Moreover, this requires the reboot of the device that in many applications is not always possible.
* **Seamless Software Updates**. Also known as A/B updates, seamless updates use one memory object to store the running object and another to store the update. All the logic to perform the update is placed in the image and the bootloader just needs to load the newer version, thus each boot will be performed at the same time. This configuration requires that the two memory objects are bootable and thus stored in the internal memory.