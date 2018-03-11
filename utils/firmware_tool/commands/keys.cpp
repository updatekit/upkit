#include "commands.hpp"
#include <fstream>
#include <string>

#ifdef WITH_TINYCRYPT
#include <tinycrypt/ecc_dsa.h>
#include <tinycrypt/ecc_dh.h>
#endif

IMPLEMENT(keys)

int keys_generate_command(Context ctx) {
    std::string subcommand = ctx.get_next_command();
    if (subcommand != "vendor" && subcommand != "server") {
        std::cout << "You can generate keys for the vendor or the server" << std::endl;
        std::cout << "Add the vendor or server command" << std::endl; // XXX this could be clear
        return EXIT_FAILURE;
    }
#if WITH_TINYDTLS
    std::cout << "The tool has been built without the key generation support" << std::endl;
    return EXIT_FAILURE;
#elif WITH_TINYCRYPT
    uint8_t priv[64];
    uint8_t pub[64];
    uECC_word_t hash_words[64];
    uECC_Curve curve = uECC_secp256r1();
    uECC_generate_random_int(hash_words, curve->n, BITS_TO_WORDS(curve->num_n_bits));
    if (!uECC_make_key(pub, priv, curve)) {
        std::cout << "Error generating keys" << std::endl;
        return EXIT_FAILURE;
    }
    if (subcommand == "vendor") {
        if (!write_binary(ctx.get_vendor_priv_key(), priv, 64)) { // XXX hardcoded value
            std::cout << "Error writing private key" << std::endl;
            return EXIT_FAILURE;
        }
        if (!write_binary(ctx.get_vendor_pub_key(), pub, 64)) { // XXX hardcoded value
            std::cout << "Error writing public key" << std::endl;
            return EXIT_FAILURE;
        }
    } else  {
        if (!write_binary(ctx.get_server_priv_key(), priv, 64)) { // XXX hardcoded value
            std::cout << "Error writing private key" << std::endl;
            return EXIT_FAILURE;
        }
        if (!write_binary(ctx.get_server_pub_key(), pub, 64)) { // XXX hardcoded value
            std::cout << "Error writing public key" << std::endl;
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
#endif
    std::cout << "The tool has been built without the key generation support" << std::endl;
    return EXIT_FAILURE;
}
