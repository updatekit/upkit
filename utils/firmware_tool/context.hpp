#include <iostream>
#include <vector>

#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_

#define FOREACH_ARG(ARG) \
    ARG(std::string, appid, 'a', "Application ID")\
    ARG(std::string, version, 'l', "Current firmware version")\
    ARG(std::string, offset, 'q', "Offset of the firmware respect the start of the file")\
    ARG(std::string, signature_file, 's', "File where the signature will be stored")\
    ARG(std::string, vendor_priv_key, 'p', "File rappresenting the vendor private key")\
    ARG(std::string, vendor_pub_key, 'c', "File rapresenting the vendor public key")\
    ARG(std::string, server_priv_key, 'k', "File rappresenting the server private key")\
    ARG(std::string, server_pub_key, 'm', "File rappresenting the server public key")\
    ARG(std::string, binary_file, 'b', "First binary file in input")\
    ARG(std::string, second_binary_file, 'z', "Second binary file in input")\
    ARG(std::string, out_file, 'f', "Output file")

#define DECLARE_ARG(type, name, ...) \
    type name;

#define DECLARE_GETTER(type, name, ...) \
    type get_##name ();

#define IMPLEMENT_GETTER(type, name, ...) \
    type Context::get_##name () { \
        return name; \
    }\

#define PRINT_ARG(type, name, arg_name, help) \
    std::cout << std::left << "   -" << std::setw(5) << arg_name \
    << std::setw(20) << #name << std::setw(40) << help << std::endl;

class Context {
    std::string prog_name;
    std::string config_file_name = "config.toml";
    std::vector<std::string> subcommands;
    uint8_t current = 0; // support at most 256 commands

    int verbosity = 0;
    FOREACH_ARG(DECLARE_ARG)

    public:
        Context(std::string);
        int parse_conf();
        int parse_arguments(int argc, char** argv);
        void print_arguments();
        int get_verbosity();

        std::string get_next_command();
        std::string get_prog_name();
        FOREACH_ARG(DECLARE_GETTER)
};

#endif /* CONTEXT_HPP_H_ */
