# Contiki-NG Platform

Libpull can be used to perform Software Updates for IoT devices using the Contiki-NG platform.

We tested and provide the following components:

* an update agent to update a Contiki-NG device using 6LoWPAN and CoAP;
* a bootloader able to manage the images and upgrade once a new image is present;

For the Contiki-NG platform we currently support the following boards:

* [Zolertia Firefly](https://zolertia.io/product/firefly/)
* [TI CC2650 SensorTag](http://www.ti.com/tool/cc2650stk)

However, you can easily integrate another board by following [this
tutorial](new_board.html).
