Welcome to libpull's documentation!
===================================

Libpull is a library to perform software updates for Internet of Things devices. The library has three main goals:
-  security: having support for the most used cryptographic libraries and hardware security modules;
- portability: it aims to be OS agnostic, support a wide number of boards and work with several network protocols;
- platform constraints: it targets Class 1 devices thus it must have a small memory footprint and reduce network bandwidth;

The library includes a bootloader and an update agent, however, it is possible to use the functions provided by the library to build your own update agent or bootloader that better fits all your platform and application needs.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   basics/basics.rst
   tutorial/tutorial.rst
   platform/platform.rst
   api/api.rst
