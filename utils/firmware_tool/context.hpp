#include <iostream>
#include <vector>

#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_

#define FOREACH_ARG(ARG) \
    ARG(std::string, platform, 'a') \
    ARG(std::string, version, 'l') \
    ARG(std::string, offset, 'q') \
    ARG(std::string, signature_file, 's') \
    ARG(std::string, vendor_priv_key, 'p') \
    ARG(std::string, vendor_pub_key, 'c') \
    ARG(std::string, server_priv_key, 'k') \
    ARG(std::string, server_pub_key, 'm') \
    ARG(std::string, binary_file, 'b') \
    ARG(std::string, out_file, 'f')

#define DECLARE_ARG(type, name, ...) \
    type name;

#define DECLARE_GETTER(type, name, ...) \
    type get_##name ();

#define IMPLEMENT_GETTER(type, name, ...) \
    type Context::get_##name () { \
        return name; \
    }\

class Context {

    std::string prog_name;
    std::string config_file_name = "config.json";
    std::vector<std::string> subcommands;
    int current = 0;

    int verbosity = 0;
    FOREACH_ARG(DECLARE_ARG)
    
    public:
        Context(std::string);
        int parse_json();
        int parse_arguments(int argc, char** argv);
        void print_arguments();
        int get_verbosity();

        int store_arguments();

        std::string get_next_command();
        std::string get_prog_name();
        FOREACH_ARG(DECLARE_GETTER)
};

#endif /* CONTEXT_HPP_H_ */
