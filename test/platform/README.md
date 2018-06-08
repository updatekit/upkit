# Platform Tests

These tests are made to be run directly on the hardware, allowing to verify
the correct implmentation of the libpull interfaces.
There are 3 tests:

- **memory.h** allows to verify the correct implementation of the memory
  interface.
- **security.h** allows to test the security functions (such as digest
  ECC key validation) directly on hardware.
- **network.h** allows to test the correct implementation of the connection
  interface.
