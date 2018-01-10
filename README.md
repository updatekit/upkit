[![Build Status](https://travis-ci.com/AntonioLangiu/Pull-IoT.svg?token=wmyzpNgdG2qdqyV8yUxq&branch=master)](https://travis-ci.com/AntonioLangiu/Pull-IoT)

Pull-IoT is a C library to implement secure and portable
updates for IoT devices. It targets [Class 1](https://tools.ietf.org/html/rfc7228#section-3)
constrained devices, characterized by ~10 kB of RAM
and ~100 kB of ROM. However, its lightweight and modular approach can be beneficial also for bigger devices.

The library can be included into MCU firmware to perform
Over The Air updates. It is composed of many high-level
modules that interact with the low-level modules implemented
by the developer using the library. This is needed because
accessing the memory and the network on embedded systems
is a complex task and each platform and architecture uses its
own primitive.

The goal of the library is to be a portable and secure solution
to reduce the costs of securing IoT devices, allowing an easy
device update and management also for very
constrained applications.

 * **Documentation**: [https://github.com/AntonioLangiu/Pull-IoT/wiki](https://github.com/AntonioLangiu/Pull-IoT/wiki)
 * **Repository**: [https://github.com/AntonioLangiu/Pull-IoT](https://github.com/AntonioLangiu/Pull-IoT)
 * **Licence**: [GPL-3.0](https://github.com/AntonioLangiu/Pull-IoT/blob/master/LICENSE)
