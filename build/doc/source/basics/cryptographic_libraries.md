# Cryptographic Libraries

The implementation of the security modules consists of high-level interfaces implemented using different cryptographic libraries. This allows to perform the signature verification without changing the code of the library, but still using different cryptographic libraries.

## Supported Libraries
Libpull currently supports three cryptographic libraries:

* [TinyDTLS](https://projects.eclipse.org/projects/iot.tinydtls) is a library that provides all the functions to instantiate a DTLS connection. It supports many cryptographic algorithms, such as Rijndael (AES), SHA256, HMAC-SHA256, ECC (with secp256r1 key). It can perform the DTLS handshake using PSK or the ECDH algorithm. It is distributed under the MIT license and maintained by the Eclipse for IoT project.
* [TinyCrypt](https://01.org/tinycrypt). It is a small-footprint cryptography library that explicitly targets constrained devices. It supports many cryptographic algorithms, such as SHA-256 hash functions, HMAC-SHA256, AES-128 (with AES-CBC, AES-CTR, and AES-CMAC encryption modes), ECC-DH key changes, and ECDSA. It is built in a modular way, allowing to include only the required modules.
* [Atmel CryptoAuthLib](https://github.com/MicrochipTech/cryptoauthlib). This library is provided by Atmel and allows to interact with their [CryptoAuthentication](https://www.microchip.com/wwwproducts/en/ATECC508A) modules. It is a very modular library and bases its function on a HAL layer in charge of communicating with the device using I2C or SPI.

## Cryptographic Libraries Memory Footprint

The choice of the cryptographic library to include was sustained by an analysis of the memory footprint of several cryptographic libraries, to identify the smallest in terms of Data and Text size.

The comparison has been performed building a simple application able to perform the verification with each library and comparing the size of the hashing and ECC functions. The output of the comparison is shown in the table above.

| Library     | SHA2 | ECC   | ECDSA |
|-------------|------|-------|-------|
| TinyDTLS    | 3800 | 7531  | 9888  |
| tinycrypt   | 3656 | 8968  | 11241 |
| PolarSSL    | 6056 | 23046 | 27735 |
| MatrixSSL   | 3864 | 29103 | 34022 |
| WolfSSL     | 4592 | 31443 | 34777 |
| LibTomCrypt | 4354 | 35959 | 38256 |

You can find more informations on the methodology used in the [specific repository](https://github.com/libpull/ecdsa_memory_footprint).