#include "commands/commands.hpp"
#include "context.hpp"

#include <map>
#include <iostream>
#include <functional>

#define FOREACH_COMMAND(COMMAND) \
    COMMAND(keys, "generate or validate keys") \
    COMMAND(manifest, "generate or validate manifest") \
    COMMAND(configs, "generate or validate configs") \
    COMMAND(help, "list the commands")

#define PRINT_HELP(name, description) \
    std::cout << "   " << #name << ":\t" << description << std::endl;

#define ADD_FUNCTION(name, description) \
    commands[#name] = name##_command;

int help_command(Context ctx) {
    std::cout << "usage: " << ctx.get_prog_name() << " command [args]" << std::endl;
    std::cout << "Available commands are:" << std::endl;
    FOREACH_COMMAND(PRINT_HELP);
    return EXIT_FAILURE;
}

int main(int argc, char** argv) {
    std::map<std::string, std::function<int(Context)>> commands;
    FOREACH_COMMAND(ADD_FUNCTION)
    Context ctx(argv[0]);;
    if (argc <= 1) {
        return help_command(ctx);
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
    return help_command(ctx);
}
