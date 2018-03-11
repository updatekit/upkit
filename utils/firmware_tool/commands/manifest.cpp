#include "commands.hpp"
#include "memory/manifest.h"
#include "security/digest.h"
#include "security/sha256.h"
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include <tinycrypt/ecc_dsa.h>

/** XXX the next lines should be removed in a refactoring */
#define BUFFER_LEN 0x1000
DIGEST_FUNC(tinycrypt);

int sign(char* private_key, uint8_t* hash, int size, uint8_t* sig);

int manifest_generate_command(Context ctx) {
    manifest_t mt;
    //manifest_init(&mt);
    // (1) Set Size
    struct stat buf;
    stat(ctx.get_binary_file().c_str(), &buf);
    set_size(&mt, buf.st_size);
    // (2) Set Platform
    std::stringstream str;
    str << ctx.get_platform();
    platform_t platform;
    str >> std::hex >> platform; // TODO check for errors
    set_platform(&mt, platform);
    // (3) Set Offset
    str.clear();
    str << ctx.get_offset();
    address_t offset;
    str >> std::hex >> offset; // TODO check for errors
    set_offset(&mt, offset);
    // (4) Set Version
    str.clear();
    str << ctx.get_version();
    version_t version;
    str >> std::hex >> version; // TODO check for errors
    set_version(&mt, version);
    // (5) Set Provisioning Server Public Key
    std::ifstream server_pub_key(ctx.get_server_pub_key(), std::ios_base::binary);
    if (!server_pub_key) {
        std::cout << "Error opening server_pub_key: " << ctx.get_server_pub_key() << std::endl;
        return EXIT_FAILURE;
    }
    char* key_buffer = new char[64];
    server_pub_key.read(key_buffer, 64); // XXX hardcoded size
    set_server_key_x(&mt, (uint8_t*) key_buffer);
    set_server_key_y(&mt, (uint8_t*) key_buffer+32);
    delete[] key_buffer;
    // (6) Calculate Firmware Hash
    digest_ctx dctx;
    digest_func digest = tinycrypt_digest_sha256;
    pull_error err = digest.init(&dctx);
    if (err) {
        std::cout << "Error initializing the hashing structure" << std::endl;
        return EXIT_FAILURE;
    }
    std::ifstream file(ctx.get_binary_file(), std::ios_base::in | std::ios_base::binary);
    if (!file) {
        std::cout << "Error opening the file:" << ctx.get_binary_file() << std::endl;
        return EXIT_FAILURE;
    }
    char* buffer = new char[BUFFER_LEN];
    while (!file.eof()) {
        file.read(buffer, BUFFER_LEN);
        err = digest.update(&dctx, buffer, file.gcount());
        if (err) {
            std::cout << "Error updating digest" << std::endl;
            delete[] buffer;
            return EXIT_FAILURE;
        }
    }
    file.close();
    delete[] buffer;
    uint8_t* hash = (uint8_t*) digest.finalize(&dctx); // TODO check the size of the hash
    set_digest(&mt, hash);
    // (6) Generate Vendor Signature
    char* vend_priv_key_buffer = new char[32];
    uint8_t* sig = new uint8_t[2*NUM_ECC_BYTES]; // XXX ardcoded value
    std::ifstream vend_priv_key(ctx.get_vendor_priv_key(), std::ios_base::binary);
    if (!vend_priv_key) {
        std::cout << "Impossible to read the vendor private key" << std::endl;
        return EXIT_FAILURE;
    }
    vend_priv_key.read(vend_priv_key_buffer, 32); // TODO check for errors
    vend_priv_key.close();
    void* manifest_buffer;
    size_t size = get_vendor_digest_buffer(&mt, &manifest_buffer);
    err = digest.init(&dctx);
    if (err) {
        std::cout << "Error initializing the hashing structure" << std::endl;
        return EXIT_FAILURE;
    }
    digest.update(&dctx, manifest_buffer, size);
    hash = (uint8_t*) digest.finalize(&dctx);
    if (sign(vend_priv_key_buffer, hash, 32, sig) != 0) {
        printf("Error generating signature\n");
        return EXIT_FAILURE;
    }
    delete[] vend_priv_key_buffer;
    // (7) Store the signature in the metadata and a standalone file
    size_t ecc_size = 32;
    set_vendor_signature_r(&mt, sig, &ecc_size); // XXX hardcoded value
    set_vendor_signature_s(&mt, sig+32, &ecc_size); // XXX hardcoded value
    std::ofstream out_signature(ctx.get_signature_file(),  std::ios_base::binary);
    if (!out_signature) {
        std::cout << "Error, impossible to write on file: " << ctx.get_signature_file() << std::endl;
        return EXIT_FAILURE;
    }
    out_signature.write((char*) sig, 64);
    out_signature.close();
    delete[] sig;
    // (8) write the metadata to the designed binary file
    std::ofstream out(ctx.get_out_file(), std::ios_base::out | std::ios_base::binary);
    if (!out) {
        std::cout << "Impossible to write on file: " << ctx.get_out_file() << std::endl;
        return EXIT_FAILURE;
    }
    out.write((char*) &mt, sizeof(manifest_t));
    out.close();
    return EXIT_SUCCESS;
}

int sign(char* private_key, uint8_t* hash, int size, uint8_t* sig) {
    if (!uECC_sign((const uint8_t*)private_key, (uint8_t *) hash, size, sig, uECC_secp256r1())) { // hardcoded curve
        std::cout << "Error during signature process" << std::endl;
        return 1;
    }
    return 0;
}

int manifest_help_command(Context ctx) {
    std::cout << "Available commands are:" << std::endl;
    std::cout << "   generate\tcreate a manifest using the input parameters" << std::endl;
    return 0;
}

int manifest_command(Context ctx) {
    std::string subcommand = ctx.get_next_command();
    if (subcommand == "generate") {
        return manifest_generate_command(ctx);
    }
    return manifest_help_command(ctx);
}
