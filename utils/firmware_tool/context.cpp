#include "context.hpp"
#include "json.hpp"
#include <getopt.h>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

#define PARSE_JSON(type, name, ...) \
    auto name = config.find(#name); \
    if (name != config.end()) { \
        this->name = *name; \
    }

int Context::parse_json() {
    json config;
    std::ifstream file(config_file_name, std::ifstream::in);
    if (!file.is_open()) {
        std::cout << "No configuration file present at path " << config_file_name << std::endl;
        return 1;
    }
    /* XXX This can throw exceptions */
    file >> config;
    FOREACH_ARG(PARSE_JSON);
    return 0;
}

#define PARSE_ARG(type, name, arg_name) \
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
    while ((opt = getopt(argc-i+1, real_argv, "va:l:s:p:c:b:f:k:m:")) != -1) {
        switch (opt) {
            case 'v': verbosity++; break;
            FOREACH_ARG(PARSE_ARG)
            default: 
              std::cout << "Invalid arg:" << real_argv[optind-1] << std::endl;
              return 1;
        }
    }
    return 0;
}

#define STORE_ARG(type, name, ...) \
    config[#name] = name;

int Context::store_arguments() {
     json config;
     std::fstream file;
     file.open (config_file_name, std::ios::out | std::ios::in);
     file >> config;
     file.seekp(std::ios_base::beg);
     FOREACH_ARG(STORE_ARG);
     file << std::setw(4) << config << std::endl;
     file.close();
     return EXIT_SUCCESS;
}

#define PRINT_ARG(type, name, ...) \
    std::cout << " " << #name << ":\t" << name << std::endl;
void Context::print_arguments() {
    std::cout << "The configured parameters are:" << std::endl;
    FOREACH_ARG(PRINT_ARG);
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
