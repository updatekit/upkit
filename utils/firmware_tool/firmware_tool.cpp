#include "commands/commands.hpp"
#include "context.hpp"

#include <map>
#include <iostream>
#include <iomanip>
#include <functional>

#define PRINT_COMMAND_HELP(name, description) \
    std::cout << "   " << #name << ":\t" << description << std::endl;

#define ADD_FUNCTION(name, description) \
    commands[#name] = name##_command;

#define PRINT_SHORT_ARG(type, name, arg_name, ...) \
    std::cout << " [-" << arg_name << " " << #name << "]"; 

int help(Context ctx) {
    std::cout << "usage: " + ctx.get_prog_name() << " <command>";
    FOREACH_ARG(PRINT_SHORT_ARG)
    std::cout << std::endl << std::endl;
    std::cout << "Available commands are:" << std::endl;
    FOREACH_COMMAND(PRINT_COMMAND_HELP)
    std::cout << std::endl;
    return EXIT_FAILURE;
}

int main(int argc, char** argv) {
    std::map<std::string, std::function<int(Context)>> commands;
    FOREACH_COMMAND(ADD_FUNCTION)
    Context ctx(argv[0]);
    if (argc <= 1) {
        return help(ctx);
    }
    auto search = commands.find(argv[1]);
    ctx.parse_arguments(argc, argv);
    ctx.parse_json();
    if (ctx.get_verbosity() > 2) {
        ctx.print_arguments();
    }
    if (search != commands.end()) {
        return search->second(ctx);
    }
    std::cout << "Invalid command" << std::endl;
    return help(ctx);
}
