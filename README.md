# UpKit

#### An Open-Source, Portable, and Lightweight Update Framework for Constrained IoT Devices

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/2d7c1102e03446a1abad33d768306677)](https://app.codacy.com/app/AntonioLangiu/libpull?utm_source=github.com&utm_medium=referral&utm_content=libpull/libpull&utm_campaign=Badge_Grade_Dashboard)
[![GitHub license](https://img.shields.io/github/license/updatekit/upkit.svg)](https://github.com/updatekit/upkit/blob/update/readme/LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/updatekit/upkit.svg)](https://github.com/updatekit/upkit/issues)
[![GitHub release](https://img.shields.io/github/release/updatekit/upkit.svg)](https://github.com/updatekit/upkit/releases)
[![Documentation Status](https://readthedocs.org/projects/libpull/badge/?version=latest)](https://libpull.readthedocs.io/en/latest/?badge=latest)
[![Build Status](https://travis-ci.org/libpull/libpull.svg?branch=master)](https://travis-ci.org/libpull/libpull)
[![Coverage Status](https://coveralls.io/repos/github/libpull/libpull/badge.svg)](https://coveralls.io/github/libpull/libpull)
[![GitHub pull-requests](https://img.shields.io/github/issues-pr/updatekit/upkit.svg)](https://github.com/updatekit/upkit/pulls)

UpKit is a C library to implement secure and portable software updates on IoT devices.

It targets [Class 1 and Class 2](https://tools.ietf.org/html/rfc7228#section-3)
constrained devices, characterized by ~10 kB of RAM and ~100 kB of ROM.
However, its lightweight and modular approach can be beneficial also for bigger devices.

The library can be included into MCU firmware to perform
Over The Air updates. It is composed of many high-level
modules that interact with the low-level abstraction layer, implemented
for the specific platform. This is needed since
accessing the memory and the network on embedded systems
is a complex task and each platform and architecture uses its
own primitive.

The goal of the library is to be a portable and secure solution
to reduce the costs of securing IoT devices, allowing an easy
device update and management also for very
constrained applications.
