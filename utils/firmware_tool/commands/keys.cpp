#include "commands.hpp"

int keys_command(Context ctx) {
    return 0;
}
/*
int generate_keys(char* private_key, char* public_key) {
     uint8_t private[NUM_ECC_BYTES];
     uint8_t public[2*NUM_ECC_BYTES];
     uECC_word_t hash_words[2*NUM_ECC_WORDS];
     // initialize
     uECC_generate_random_int(hash_words, curve->n, BITS_TO_WORDS(curve->num_n_bits));
     // generate keys
     printf("Generating keys...\n");
     if (!uECC_make_key(public, private, curve)) {
         printf("Error: impossible to generate keys\n");
         return 1;
     }
     // print to stdout
     print_buf("private", private, NUM_ECC_BYTES);
     print_buf("public", public, 2*NUM_ECC_BYTES);
     // store as binary
     if (binary_writer(public_key, public, 2*NUM_ECC_BYTES) != 0) {
         printf ("Error: public key not correcly written\n");
         return 1;
     }
     if (binary_writer(private_key, private, NUM_ECC_BYTES) != 0) {
         printf ("Error: private key not correcly written\n");
         return 1;
     }
     printf("Generating keys...done\n");
     return 0;
 }

 int validate_key(uint8_t *public, uint8_t *hash, int size, uint8_t *sig) {
     if (!uECC_verify(public, (uint8_t *) hash, size, sig, curve)) {
         printf ("Error: Invalid signature!\n");
         return 1;
     }
     printf("The signature is valid\n");
     return 0;
 }
 */
