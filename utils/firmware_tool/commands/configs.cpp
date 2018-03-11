#include "commands.hpp"
#include "../json.hpp"
#include <fstream>

using json = nlohmann::json;

int configs_print_command(Context ctx) {
    ctx.print_arguments();
    return EXIT_SUCCESS;
}

int configs_store_command(Context ctx) {
    return ctx.store_arguments();
}

int configs_validate_command(Context ctx) {
    // TODO implement
    return 0;
}

int configs_command(Context ctx) {
    std::string subcommand = ctx.get_next_command();
    if (subcommand == "print") {
        return configs_print_command(ctx);
    } else if (subcommand == "store") {
        return configs_store_command(ctx);
    } else if (subcommand == "") {
        return configs_print_command(ctx);
    } else if (subcommand == "validate") {
        return configs_validate_command(ctx);
    }
    std::cout << "Not recognized subcommand" << std::endl;
    return EXIT_FAILURE;
}
