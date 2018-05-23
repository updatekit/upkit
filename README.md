![Libpull logo](https://github.com/AntonioLangiu/libpull/raw/master/libpull.png)

 # The Portable Updates Lightweight Library

 [![Build Status](https://travis-ci.com/AntonioLangiu/libpull.svg?token=wmyzpNgdG2qdqyV8yUxq&branch=master)](https://travis-ci.com/AntonioLangiu/libpull)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Flibpull%2Flibpull.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Flibpull%2Flibpull?ref=badge_shield)


 libpull is a C library to implement secure and portable updates on IoT devices.
 It targets [Class 1](https://tools.ietf.org/html/rfc7228#section-3)
 constrained devices, characterized by ~10 kB of RAM and ~100 kB of ROM.
 However, its lightweight and modular approach can be beneficial also for bigger devices.

 The library can be included into MCU firmware to perform
 Over The Air updates. It is composed of many high-level
 modules that interact with the low-level interfaces, implemented
 for the specific platform. This is needed since
 accessing the memory and the network on embedded systems
 is a complex task and each platform and architecture uses its
 own primitive.

 The goal of the library is to be a portable and secure solution
 to reduce the costs of securing IoT devices, allowing an easy
 device update and management also for very
 constrained applications.


 ⚠  **Version: 0.0.1 - The API and the modules can change at any point until we
 reach a stable version.**


## License
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Flibpull%2Flibpull.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Flibpull%2Flibpull?ref=badge_large)