# Bootloader

The initial bootloader for this update system will be based on
another Contiki application.

The bootloader is in charge of checking if in the external
memory there is a newer version of the firmware and jump to
it in that case.

The bootloader must be very constrained in memory and device
usage. It does not support network and it is in charge of
performing some bootstrapping operations the first time the
system boots.
