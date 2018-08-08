Welcome to libpull's documentation!
===================================

.. mdinclude:: badges.md

Libpull is a library to perform software updates for Internet of Things devices. The library is build around three main principles:

- **Security**: having support for the most used cryptographic libraries and hardware security modules;
- **Portability**: aims to be OS agnostic, support a wide number of boards and work with several network protocols;
- **Platform constraints**: aims to targets Class 1 devices, thus having a small memory footprint and reducing network bandwidth;

The library includes a bootloader and an update agent, however, it is possible to use the functions provided by the library to build your own agent that better fits all your platform and application needs.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   basics/basics.rst
   tutorial/tutorial.rst
   platform/platform.rst
   api/api.rst
