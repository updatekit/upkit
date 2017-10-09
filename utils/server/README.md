# Testing Server

This server will be used to test the library. It should
implement a set of CoAP resources and expose them over
UDP (on the default CoAP port 5683) and on DTLS (on
the default CoAPS port 5684).

This server does not have the goal of manage the device and
will just provide some simple provisioning resource. This
means that the server will expose just 
some simple provisioning resource, one based on polling and one
on the subscribe feature. The management of the device should
be done with more suitable protocols such as OMA LWM2M.

## Supported platforms

The server works on posix systems and is based on libcoap
to provide an easy integration with tinydtls and OpenSSL.
However it will support initially just the tinydtls library
to perform encryption and Elliptic-curve Diffie-Hellman.

## Exposed resources

<table>
  <tr>
    <th class="tg-031e">Resource</th>
    <th class="tg-yw4l">Method</th>
    <th class="tg-yw4l">Usage</th>
    <th class="tg-031e">Returns</th>
  </tr>
  <tr>
    <td class="tg-031e">echo</td>
    <td class="tg-yw4l">GET</td>
    <td class="tg-yw4l">Useful for testing purposes</td>
    <td class="tg-031e">Sends back the received payload</td>
  </tr>
  <tr>
    <td class="tg-031e">version</td>
    <td class="tg-yw4l">GET</td>
    <td class="tg-yw4l">Get the latest version available</td>
    <td class="tg-031e">uint16_t representing the version number</td>
  </tr>
  <tr>
    <td class="tg-031e">version</td>
    <td class="tg-yw4l">SUBSCRIBE</td>
    <td class="tg-yw4l">Get the latest version available</td>
    <td class="tg-031e">uint16_t representing the version number</td>
  </tr>
  <tr>
    <td class="tg-yw4l">version/invalid</td>
    <td class="tg-yw4l">GET</td>
    <td class="tg-yw4l">Get an invalid version to test if the library is able to manage it.</td>
    <td class="tg-yw4l">uint16_t with value 0x0</td>
  </tr>
  <tr>
    <td class="tg-yw4l">version/invalid</td>
    <td class="tg-yw4l">SUBSCRIBE</td>
    <td class="tg-yw4l">Get an invalid version to test if the library is able to manage it</td>
    <td class="tg-yw4l">uint16_t with value 0x0</td>
  </tr>
  <tr>
    <td class="tg-yw4l">firmware</td>
    <td class="tg-yw4l">GET</td>
    <td class="tg-yw4l">Get the latest firmware available</td>
    <td class="tg-yw4l">A Blockwise transfer of the latest firmware.</td>
  </tr>
  <tr>
    <td class="tg-yw4l">firmware/[current_version]</td>
    <td class="tg-yw4l">GET</td>
    <td class="tg-yw4l">Get the latest firmware delta. The current version is needed to generate a delta between the last and current version.</td>
    <td class="tg-yw4l">A Blockwise transfer of the delta between the latest and the current version.</td>
  </tr>
  <tr>
    <td class="tg-yw4l">firmware_invalid/signature</td>
    <td class="tg-yw4l">GET</td>
    <td class="tg-yw4l">Get an invalid firmware to test if the library is able to manage it.</td>
    <td class="tg-yw4l">Returns an invalid firmware, where the signature is not valid</td>
  </tr>
  <tr>
    <td class="tg-yw4l">firmware_invalid/size</td>
    <td class="tg-yw4l">GET</td>
    <td class="tg-yw4l">Get an invalid firmware to test if the library is able to manage it.</td>
    <td class="tg-yw4l">Returns an invalid firmware, where the size does not match the sent data</td>
  </tr>
</table>
