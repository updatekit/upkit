![Libpull logo](https://github.com/libpull/libpull_graphics/raw/master/logo/libpull.png)

# The Portable Updates Lightweight Library

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/2d7c1102e03446a1abad33d768306677)](https://app.codacy.com/app/AntonioLangiu/libpull?utm_source=github.com&utm_medium=referral&utm_content=libpull/libpull&utm_campaign=Badge_Grade_Dashboard)
[![GitHub license](https://img.shields.io/github/license/libpull/libpull.svg)](https://github.com/libpull/libpull/blob/update/readme/LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/libpull/libpull.svg)](https://github.com/libpull/libpull/issues)
[![GitHub release](https://img.shields.io/github/release/libpull/libpull.svg)](https://github.com/libpull/libpull/releases)
[![Documentation Status](https://readthedocs.org/projects/libpull/badge/?version=latest)](https://libpull.readthedocs.io/en/latest/?badge=latest)
[![Build Status](https://travis-ci.org/libpull/libpull.svg?branch=master)](https://travis-ci.org/libpull/libpull)
[![Coverage Status](https://coveralls.io/repos/github/libpull/libpull/badge.svg)](https://coveralls.io/github/libpull/libpull)
[![CodeFactor](https://www.codefactor.io/repository/github/libpull/libpull/badge)](https://www.codefactor.io/repository/github/libpull/libpull)
[![GitHub pull-requests](https://img.shields.io/github/issues-pr/libpull/libpull.svg)](https://github.com/libpull/libpull/pulls)

libpull is a C library to implement secure and portable software updates on IoT devices.

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

## Online Presence

- **Documentation**:  [libpull.readthedocs.io](http://libpull.readthedocs.io)
- **Twitter**:  [@libpull](https://twitter.com/libpull)
- **Join Slack**:  [libpull.slack.com](https://join.slack.com/t/libpull/shared_invite/enQtNDEyOTkzMjE5MDkxLTEyZjUxZGE4MDYwNzE4ODdmYzFkZTA1NDAyN2ExOGVlYzQ3ODhkNTk1ZTZjMDExMmE1NjhiNmUyYjRmN2ZkYzc)
