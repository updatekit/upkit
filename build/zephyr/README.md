# Zephyr Platform

Libpull can be used to perform Software Updates for IoT devices using the Zephyr platform.

We tested and provide the following components:

* an update agent to update a Zephyr device using OpenThread;
* an update agent to update a Zephyr device using
Bluetooth Low Energy (BLE);
* a bootloader able to manage the images and upgrade once a new image is present;

For the Zephyr platform we currently support the following boards:

* [Nordic nRF52840](https://www.nordicsemi.com/eng/Products/nRF52840)

However, you can easily integrate another board by following [this tutorial](new_board.html).
