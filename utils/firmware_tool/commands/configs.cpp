#include "commands.hpp"
#include <fstream>

IMPLEMENT(configs)

int configs_store_command(Context ctx) {
    return ctx.store_arguments();
}

int configs_validate_command(Context ctx) {
    // TODO implement
    return 0;
}

int configs_print_command(Context ctx) {
    ctx.print_arguments();
    return 0;
}
