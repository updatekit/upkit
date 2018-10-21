#include "context.hpp"
#include "cpptoml.h"
#include <getopt.h>
#include <iostream>
#include <fstream>

#define PARSE_TOML_ELEM(type, name, ...) \
    if (this->name == "" &&  (val = config->get_as<std::string>(#name))) {;\
        this->name = *val; \
    }

int Context::parse_conf() {
    std::shared_ptr<cpptoml::table> config;
    try {
    config = cpptoml::parse_file(config_file_name);
    } catch (cpptoml::parse_exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    cpptoml::option<std::string> val;
    FOREACH_ARG(PARSE_TOML_ELEM);
    return 0;
}

#define PARSE_ARG(type, name, arg_name, ...) \
    case arg_name: name = std::string(real_argv[optind-1]); break;

int Context::parse_arguments(int argc, char** argv) {
    prog_name = std::string(argv[0]);
    int opt = 0, i;
    for (i = 2; i < argc; i++) {
        if (argv[i][0] == '-')
            break;
        subcommands.push_back(argv[i]);
    }
    if (i >= argc) {
        // No specific arguments to parse
        return 0;
    }
    char** real_argv = &argv[i-1];
    while ((opt = getopt(argc-i+1, real_argv, "va:l:s:p:c:b:f:k:m:y:z:")) != -1) {
        switch (opt) {
            case 'v': verbosity++; break;
            case 'y': config_file_name = std::string(real_argv[optind-1]); break;
            FOREACH_ARG(PARSE_ARG)
            default:
              std::cout << "Invalid arg:" << real_argv[optind-1] << std::endl;
              return 1;
        }
    }
    return 0;
}

#define PRINT_ARG_VALUE(type, name, ...) \
    std::cout << std::left << " " << std::setw(20) << #name  \
    << std::setw(20) << std::left << name << std::endl;
void Context::print_arguments() {
    std::cout << "The configured parameters are:" << std::endl;
    FOREACH_ARG(PRINT_ARG_VALUE);
}

Context::Context(std::string name) {
    prog_name = name;
}

std::string Context::get_next_command() {
    return current < subcommands.size()? subcommands[current++] : "";
}

std::string Context::get_prog_name() {
    return prog_name;
}

int Context::get_verbosity() {
    return verbosity;
}

FOREACH_ARG(IMPLEMENT_GETTER)
