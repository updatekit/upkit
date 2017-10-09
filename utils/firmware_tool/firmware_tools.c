#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "memory/metadata.h"

#include <tinycrypt/ecc_dsa.h>
#include <tinycrypt/ecc_dh.h>
#include <tinycrypt/sha256.h>

#define BUFFER_LEN 1024

/*************************************************/
/* Change this to set the default parameters     */
/*************************************************/
char* vend_pub_key_g = "vend_key.pub";
char* vend_priv_key_g = "vend_key.priv";
char* signature_g = "signature.bin";
char* metadata_g = "metadata.bin";
char* binary_file_g = "firmware.hex";
char* metadata_bin_g = "metadata.bin";
char* prov_priv_key_g = "prov_key.priv";
char* prov_pub_key_g = "prov_key.pub";

// Prototypes
int generate_keys(char*, char*);
int binary_writer(char*, uint8_t*, int);
int binary_reader(char*, uint8_t*, int);
void print_hex(char*, unsigned char*, int);
static void print_buf(char*, unsigned char*, int);
int sign(uint8_t*, uint8_t*, int, uint8_t*);
int hash_calculator(char*, uint8_t*);
void help(char*);
int check_exists(char*);

// Global variables
const struct uECC_Curve_t * curve;
int verbose = 0;

// Enumerations
enum output_type {
    BINARY,
    TEXTUAL
};

int check_exists(char* path) {
    struct stat buf;
    return !stat(path, &buf);
}

int main(int argc, char** argv) {
    int opt = 0;
    opterr = 0;
    curve = uECC_secp256r1();
    // Operations
    // Parameters
    char* version = NULL;
    char* platform = NULL;
    char* out_file = NULL;
    char* signature = signature_g;
    char* vend_priv_key = vend_priv_key_g;
    char* vend_pub_key = vend_pub_key_g;
    char* binary_file = binary_file_g;
    char* prov_priv_key = prov_priv_key_g;
    char* prov_pub_key = prov_pub_key_g;
    if (argc == 1) {
        help(argv[0]);
        return EXIT_FAILURE;
    }
    while ((opt = getopt (argc, argv, "va:l:s:p:c:b:f:k:m:")) != -1) {
        switch (opt) {
            // Parameters
            case 'v': verbose++; break; // verbosity
            case 'a': platform = argv[optind-1]; break; // version
            case 'l': version = argv[optind-1]; break; // version
            case 's': signature = argv[optind-1]; break; // signature
            case 'p': vend_priv_key = argv[optind-1]; break; // vendor private key
            case 'c': vend_pub_key = argv[optind-1]; break; // vendor public key
            case 'b': binary_file = argv[optind-1]; break; // firmware binary
            case 'f': out_file = argv[optind-1]; break; // output file
            case 'k': prov_priv_key = argv[optind-1]; break; // provisioning private key
            case 'm': prov_pub_key = argv[optind-1]; break; // provisioning public key
            //case 'h':  hextobin(optarg, hash, NUM_ECC_BYTES); break; // hash as string
            default: help(argv[0]); return EXIT_FAILURE;
        }
    }
    if (verbose >= 2) {
        printf("Params:\n");
        printf(" version:\t%s\n", version);
        printf(" signature:\t%s [%sexists]\n", signature, check_exists(signature)?"":"not ");
        printf(" vendor private_key:\t%s [%sexists]\n", vend_priv_key, check_exists(vend_priv_key)?"":"not ");
        printf(" vendor public_key:\t%s [%sexists]\n", vend_pub_key, check_exists(vend_pub_key)?"":"not ");
        printf(" binary_file:\t%s [%sexists]\n", binary_file, check_exists(binary_file)?"":"not ");
        printf(" provisioning private_key:\t%s [%sexists]\n", prov_priv_key, check_exists(prov_priv_key)?"":"not ");
        printf(" provisioning public_key:\t%s [%sexists]\n", prov_pub_key, check_exists(prov_pub_key)?"":"not ");
        printf(" out_file:\t%s\n", out_file == NULL? "not given": out_file);
    }
    // XXX you should be sure that optind exists
    if (strcmp(argv[optind], "gen_vend_keys") == 0) {
        return generate_keys(vend_priv_key, vend_pub_key);
    } else if (strcmp(argv[optind], "gen_prov_keys") == 0) {
        return generate_keys(prov_priv_key, prov_pub_key);
    } else if (strcmp(argv[optind], "metadata") == 0) {
        if (!check_exists(vend_priv_key)) {
            printf("Vendor private key not valid: %s\n", vend_priv_key);
        } else if (!check_exists(vend_pub_key)) {
            printf("Vendor public key not valid: %s\n", vend_pub_key);
            return EXIT_FAILURE;
        } else if (!check_exists(prov_priv_key)) {
            printf("Provsining server private key not valid: %s\n", prov_priv_key);
        } else if (!check_exists(prov_pub_key)) {
            printf("Provisioning server public key not valid: %s\n", prov_pub_key);
            return EXIT_FAILURE;
        } else if (!version) {
            printf("You should provide a valid version\n");
            return EXIT_FAILURE;
        } else if (!check_exists(binary_file)) {
            printf("The binary file does not exists: %s\n", binary_file);
            return EXIT_FAILURE;
        } else if (!platform) {
            printf("You should provide a platform type\n");
            return EXIT_FAILURE;
        }
        if (out_file == NULL) {
            printf("Using default out file\n");
            out_file = metadata_bin_g;
        }
        metadata mt;
        bzero(&mt, sizeof(metadata));
        // Set Size
        struct stat buf;
        stat(binary_file, &buf);
        printf("%lld\n", buf.st_size);
        mt.vendor.size = buf.st_size;
        printf("Size of binary file is %u\n", mt.vendor.size);
        // Set Platform XXX check the length
        sscanf(platform, "%hx", (short unsigned int*) &(mt.vendor.platform));
        printf("Using platform %hx\n", mt.vendor.platform);
        // XXX Add an options for this hardcoded value
        mt.vendor.offset = 0x100;
        // Set Version XXX check the length
        sscanf(version, "%hx", (short unsigned int*) &(mt.vendor.version));
        // Set Provisioning public key
        uint8_t value[64];
        binary_reader(prov_pub_key, (uint8_t*) &value, 64);
        memcpy(&mt.vendor.server_key_x, value, 32);
        memcpy(&mt.vendor.server_key_y, value+32, 32);
        // Generate Vendor Signature
        int len;
        uint8_t buffer[BUFFER_LEN];
        struct tc_sha256_state_struct s;
        uint8_t hash[32];
        uint8_t vend_priv_key_bin[32];
        int fin = open(binary_file, O_RDONLY);
        if (fin < 0) {
            printf ("Impossible to read the file %s\n", binary_file);
            return EXIT_FAILURE;
        }
        if (binary_reader(vend_priv_key, vend_priv_key_bin, 32) != 0) {
            printf("Impossible to read the vendor private key\n");
            return EXIT_FAILURE;
        }
        tc_sha256_init(&s);
        int buffer_len = to_digest(&mt, buffer, BUFFER_LEN);
        if (buffer_len < 0) {
            printf("Error adding the metadata to the digest\n");
            return -1;
        }
        tc_sha256_update(&s, buffer, buffer_len);
        while ((len = read(fin, buffer, BUFFER_LEN)) > 0) {
            tc_sha256_update(&s, (const uint8_t *) buffer, len);
        }
        tc_sha256_final(hash, &s);
        print_buf("Calculated hash", hash, 32);
        uint8_t sig[2*NUM_ECC_BYTES];
        if (sign(vend_priv_key_bin, hash, 32, sig) != 0) {
            printf("Error generating signature\n");
            return EXIT_FAILURE;
        }
        if (binary_writer(signature, sig, 2*NUM_ECC_BYTES) != 0) {
            printf ("Error: signature not correcly written\n");
            return 1;
        }
        memcpy(mt.vendor_signature_r, sig, 32);
        memcpy(mt.vendor_signature_s, sig+32, 32);
        printf("Writing Metadata...\n");
        if (binary_writer(out_file, (uint8_t*) &mt, sizeof(mt)) != 0) {
            printf("Error writing metadata\n");
            return EXIT_FAILURE;
        }
        close(fin);
        return EXIT_SUCCESS;
    }
    printf("Invalid command\n");
    return EXIT_FAILURE;
}

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

int binary_writer(char *file_name, uint8_t*buffer, int len) {
    int file = open(file_name, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (file < 0) {
        printf ("Error: impossible to write the file %s\n", file_name);
        return 1;
    }
    if (write(file, buffer, len) != len) {
        printf ("Error: file not correcly written\n");
        return 1;
    }
    close(file);
    return 0;
}

int binary_reader(char *file_name, uint8_t*buffer, int len) {
    int file = open(file_name, O_RDONLY);
    if (file < 0) {
        printf ("Impossible to read the file %s\n", file_name);
        return 1;
    }
    if (read(file, buffer, len) != len) {
        printf ("Error, impossible to read the file!\n");
        return 1;
    }
    close(file);
    return 0;
}

void print_hex(char *title, unsigned char *buf, int buf_len) {
    int i = 0;
    fprintf(stdout, "%s\n", title);
    for(i = 0; i < buf_len; ++i) {
        fprintf(stdout, "0x%02x%s", buf[i], ( i + 1 ) % 16 == 0 ? ",\r\n" : "," );
    }
    printf("\n");
}

int sign(uint8_t * private, uint8_t* hash, int size, uint8_t* sig) {
    print_buf("hash", hash, NUM_ECC_BYTES);
    if (!uECC_sign(private, (uint8_t *) hash, size, sig, curve)) {
        printf("Error during signature process\n");
        return 1;
    }

    print_buf("sig", sig, 2*NUM_ECC_BYTES);
    return 0;
}

int hash_calculator(char * file_name, uint8_t *hash) {
    int len;
    char * buffer[BUFFER_LEN];
    struct tc_sha256_state_struct s;
    int file = open(file_name, O_RDONLY);
    if (file < 0) {
        printf ("Impossible to read the file %s\n", file_name);
        return 1;
    }
    tc_sha256_init(&s);
    while ((len = read(file, buffer, BUFFER_LEN)) > 0) {
        tc_sha256_update(&s, (const uint8_t *) buffer, len);
        if (len < BUFFER_LEN) {
            break;
        }
    }
    close(file);
    tc_sha256_final(hash, &s);
    print_buf("hash_calculator", hash, NUM_ECC_BYTES);
    return 0;
}

void help(char * prog_name) {
    fprintf(stderr, "Usage: %s [-kvspcbf] [command]\n", prog_name);
    fprintf(stderr, "available commands are: ");
}

static void print_buf(char *title, unsigned char *buf, int buf_len) {
    int i = 0;
    fprintf(stdout, "%s\n", title);
    for(i = 0; i < buf_len; ++i) {
        fprintf(stdout, "0x%02x%s", buf[i], ( i + 1 ) % 16 == 0 ? ",\r\n" : "," );
    }
    printf("\n");
}
