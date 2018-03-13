#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string.h>
#include "commands.hpp"
#include "memory/manifest.h"
#include "security/digest.h"
#include "security/sha256.h"

/** XXX the next lines should be removed in a refactoring */
#define BUFFER_LEN 0x1000
DIGEST_FUNC(tinycrypt);

IMPLEMENT(manifest)

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
    uint8_t* vend_priv_key_buffer = new uint8_t[32];
    uint8_t* signature = new uint8_t[64]; // XXX hardcoded value
    std::ifstream vend_priv_key(ctx.get_vendor_priv_key(), std::ios_base::binary);
    if (!vend_priv_key) {
        std::cout << "Impossible to read the vendor private key" << std::endl;
        return EXIT_FAILURE;
    }
    vend_priv_key.read((char*)vend_priv_key_buffer, 32); // TODO check for errors && XXX hardcoded value
    vend_priv_key.close();
 
    err = sign_manifest_vendor(&mt, digest, vend_priv_key_buffer, signature, secp256r1);
    if (err) {
        std::cout << "Error signing the manifest" << std::endl;
        return EXIT_FAILURE;
    }
    delete[] vend_priv_key_buffer;
    // (7) Store the signature in the metadata and a standalone file
    std::ofstream out_signature(ctx.get_signature_file(),  std::ios_base::binary);
    if (!out_signature) {
        std::cout << "Error writing on file: " << ctx.get_signature_file() << std::endl;
        return EXIT_FAILURE;
    }
    out_signature.write((char*) signature, 64);
    out_signature.close();
    delete[] signature;
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

static void show_buffer(std::string name, const unsigned char* data, size_t data_len, size_t offset) {
    std::cout << "   " << name << ":";
    std::cout << std::setw(offset - name.length() - 4) << " ";
    print_buf(data, data_len, offset);
    std::cout << std::setfill(' ');
}

int manifest_print_command(Context ctx) {
    std::ifstream input(ctx.get_out_file(), std::ios_base::binary);
    if (!input) {
        std::cerr << "Error opening firmware file: " << ctx.get_out_file() << std::endl;
        return EXIT_FAILURE;
    }
    manifest_t mt;
    input.read((char*) &mt, sizeof(mt));
    std::cout << ctx.get_out_file() << std::endl;
    std::cout << "   version:\t  " << "0x" << std::hex << get_version(&mt) << std::endl;
    std::cout << "   platform:\t  " << "0x" << std::hex << get_platform(&mt) << std::endl;
    std::cout << "   size:\t  " << std::to_string(get_size(&mt)) << " B" <<  std::endl;
    std::cout << "   offset:\t  " << "0x" << std::hex << get_offset(&mt) << " B" << std::endl;
    size_t offset = 18;
    show_buffer("server_key_x", get_server_key_x(&mt), 32, offset);
    show_buffer("server_key_y", get_server_key_y(&mt), 32, offset);
    show_buffer("digest", get_digest(&mt), 32, offset);
    uint8_t size;
    uint8_t* signature = get_vendor_signature_r(&mt, &size);
    show_buffer("vendor_sig_r", signature, size, offset);
    signature = get_vendor_signature_s(&mt, &size);
    show_buffer("vendor_sig_s", signature, size, offset);
    signature = get_server_signature_r(&mt, &size);
    show_buffer("server_sig_r", signature, size, offset);
    signature = get_server_signature_s(&mt, &size);
    show_buffer("server_sig_s", signature, size, offset);
    return EXIT_SUCCESS;
}

static void log_item(std::string name, bool value) {
    std::cout << "   " << name << ":\t[" << ((!value)? "in":"") << "valid]" << std::endl;
}

int manifest_validate_command(Context ctx) {
    std::cout << ctx.get_out_file() << std::endl;
    manifest_t mt;
    //manifest_init(&mt);
    // (1) Read the manifest from the file
    std::ifstream input(ctx.get_out_file(), std::ios_base::binary);
    if (!input) {
        std::cout << "Error reading manifest from file: " << ctx.get_out_file() << std::endl;
        return EXIT_FAILURE;
    }
    input.read((char*) &mt, sizeof(manifest_t));
    input.close();
    
    // (2) Validate Size
    struct stat buf;
    stat(ctx.get_binary_file().c_str(), &buf);
    log_item("size", get_size(&mt) == buf.st_size);
    // (3) Validate Digest
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
    log_item("digest", memcmp(get_digest(&mt), hash, digest.size) == 0);
    // (4) Validate Server Public Key
    std::ifstream server_pub_key(ctx.get_server_pub_key(), std::ios_base::binary);
    if (!server_pub_key) {
        std::cout << "Error opening server_pub_key: " << ctx.get_server_pub_key() << std::endl;
        return EXIT_FAILURE;
    }
    uint8_t* server_public_key_buffer = new uint8_t[64];
    server_pub_key.read((char*)server_public_key_buffer, 64); // XXX hardcoded size
    server_pub_key.close();
    log_item("server_key_x", memcmp(get_server_key_x(&mt), server_public_key_buffer, 32) == 0);
    log_item("server_key_y", memcmp(get_server_key_y(&mt), server_public_key_buffer+32, 32) == 0);
    // (5) Validate Vendor Signature
    std::ifstream vendor_pub_key(ctx.get_vendor_pub_key(), std::ios_base::binary);
    if (!server_pub_key) {
        std::cout << "Error opening vendor_pub_key: " << ctx.get_server_pub_key() << std::endl;
        return EXIT_FAILURE;
    }
    uint8_t* vendor_public_key_buffer = new uint8_t[64];
    vendor_pub_key.read((char*)vendor_public_key_buffer, 64); // XXX hardcoded size
    vendor_pub_key.close();
    err = verify_manifest_vendor(&mt, digest, vendor_public_key_buffer,
                                         vendor_public_key_buffer+32, secp256r1);
    log_item("vendor_signature", err == PULL_SUCCESS);
    delete[] vendor_public_key_buffer;
    // (6) Validate Server Signature
    err = verify_manifest_server(&mt, digest, server_public_key_buffer,
                                         server_public_key_buffer+32, secp256r1);
    log_item("server_signature", err == PULL_SUCCESS);
    delete[] server_public_key_buffer;
    return EXIT_SUCCESS;
}

