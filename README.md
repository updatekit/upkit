![Libpull logo](https://github.com/AntonioLangiu/libpull/raw/master/libpull.png)

# The Portable Updates Lightweight Library

[![GitHub license](https://img.shields.io/github/license/libpull/libpull.svg)](https://github.com/libpull/libpull/blob/update/readme/LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/libpull/libpull.svg)](https://github.com/libpull/libpull/issues)
[![GitHub release](https://img.shields.io/github/release/libpull/libpull.svg)](https://github.com/libpull/libpull/releases)

[![Build Status](https://travis-ci.org/libpull/libpull.svg?branch=master)](https://travis-ci.org/libpull/libpull)
[![codecov](https://codecov.io/gh/libpull/libpull/branch/master/graph/badge.svg)](https://codecov.io/gh/libpull/libpull)
[![GitHub pull-requests](https://img.shields.io/github/issues-pr/libpull/libpull.svg)](https://github.com/libpull/libpull/pull/)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](https://github.com/libpull/libpull/pulls)



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
