# Pull-IoT Portable Updates Lightweight Library

[![Build Status](https://travis-ci.com/AntonioLangiu/pull-iot.svg?token=wmyzpNgdG2qdqyV8yUxq&branch=master)](https://travis-ci.com/AntonioLangiu/pull-iot)

Pull-IoT is a C library to implement secure and portable
updates for IoT devices. It targets [Class 1](https://tools.ietf.org/html/rfc7228#section-3)
constrained devices, characterized by ~10 kB of RAM
and ~100 kB of ROM, however its lightweight and modular
approach can be beneficial also on bigger devices.

The library can be included into MCU firmware to perform
Over The Air updates. It is composed by many high level
modules that interact with the low level modules that must
be plugged by the final user according to the platform and
the OS in use.

The library does not assume any Transport method, and defines
an interface that can be implemented with many protocols, for example
CoAP or MQTT when dealing with constraint devices or even HTTP
with more powerful one.

## General Principles

 - OS agnostic: the library does not rely on any OS specific functionality;
 - No dynamic memory: the library can be used on embedded systems without
   dynamic memory;
 - No specific transport: the library does not assume any transport protocol
   to download the update;
 - Modular: the library is divided into modules that can be plugged to eneble
   specific functionalities;

## Software Architecture

The software update process for embedded systems has normally to deal with
hardware or very low level problems, such as accessing the flash or interacting
with the network. For this reason, the library defines two interfaces that must
be implemented according to the platform needs.

The library has been designed in a modular way, to make it compatible with many
architectures and platform needs. The defined modules are:

 -   Network: This module in charge of communicating
     with the network and includes components such as the
     `subscriber`,  used to discover the presence of an
     update and the `receiver`, used to download and
     store the available update. This module rely on on the transport
     interface that must be implemented by the developer according
     to its needs.

 -   Memory: This module rappresent an abstraction of the
     memory, allowing the library to interact with a `memory_object`
     without having to deal with the low level details. In this way
     can easily work with files or raw flash access without changing
     the API.
     
 -   Security: This module includes all the memory functions used to
     secure the update process. It includes abstraction to perform the
     digital signature process, based on wrapper that makes it easy to
     change the cryptographic library according to the platform needs.
     
## Dependencies

To being able to link the test you need to have the following
libraries:

 - [libcoap](https://libcoap.net/)

## Testing

The testing process is based on 
[ceedling](http://www.throwtheswitch.org/ceedling/), a unit
testing tool that makes it easy to wrap functions in a portable way,
being compatible with Linux and OSX. To run unit testing you need to have
`ruby` installed. To install `ceedling` you can use `gem`, the ruby
packet manager.

    gem install ceedling

To run all the tests

    ceeding test:all

To run just a specific test
    
    ceedling test:test_name

To create a new test for a module you can type

    ceedling module:create[folder/file]

and it will automatically create the test and the source
files for the new module. Ceedling can also be used to calculate
the code coverage.

    ceedling gcov:all
