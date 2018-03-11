#include "commands.hpp"
#include <fstream>
#include <string>

#ifdef WITH_TINYCRYPT
#include <tinycrypt/ecc_dsa.h>
#include <tinycrypt/ecc_dh.h>
#endif

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

    std::ofstream priv_out(ctx.get_server_priv_key(), std::ios_base::binary);
    if (!priv_out) {
        std::cout << "Error opening output file: " << ctx.get_server_priv_key() << std::endl;
        return EXIT_FAILURE;
    }
    priv_out.write((char*)priv, 64);
    priv_out.close();

    std::ofstream pub_out(ctx.get_server_pub_key(), std::ios_base::binary);
    if (!pub_out) {
        std::cout << "Error opening output file: " << ctx.get_server_pub_key() << std::endl;
        return EXIT_FAILURE;
    }
    pub_out.write((char*)pub, 64);
    pub_out.close();
    return EXIT_SUCCESS;
#endif
    std::cout << "The tool has been built without the key generation support" << std::endl;
    return EXIT_FAILURE;
}

int keys_help_command(Context ctx) {
    // TODO implement
    return EXIT_SUCCESS;
}

int keys_command(Context ctx) {
    std::string subcommand = ctx.get_next_command();
    if (subcommand == "generate") {
        return keys_generate_command(ctx);
    } else if (subcommand == "help") {
        return keys_help_command(ctx);
    }
    return !keys_help_command(ctx);
}
