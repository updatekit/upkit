#include "commands.hpp"
#include <iostream>

#define PRINT_COMMAND_FULL(name, description) \
        std::cout << std::left << "   " << std::setw(15) << #name \
        << std::setw(40) << description << std::endl; \
        FOREACH_##name##_COMMAND(PRINT_HELP) \
        std::cout << std::endl;

int help_command(Context ctx) {
    std::cout << "This firmware_tool utility can be used to generate";
    std::cout << "update images compatible with libtool." << std::endl;
    std::cout << "It supports the following commands: " << std::endl;
    FOREACH_COMMAND(PRINT_COMMAND_FULL)
    std::cout << "The tool accepts parameter passing them as arguments or setting" << std::endl;
    std::cout << "a configuration JSON file. The parameters are listed below, indicating" << std::endl;
    std::cout << "also the key that should be setted in the JSON configuration file." << std::endl;
    FOREACH_ARG(PRINT_ARG)
    return EXIT_SUCCESS;
}

