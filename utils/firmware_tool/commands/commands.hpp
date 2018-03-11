#include "../context.hpp"

#define FOREACH_COMMAND(COMMAND) \
    COMMAND(keys, "manage cryptographic keys") \
    COMMAND(manifest, "interact with the manifest") \
    COMMAND(configs, "interact with the stored configuration") \
    COMMAND(help, "list the commands and the parameters")

#define FOREACH_keys_COMMAND(COMMAND) \
    COMMAND(keys, generate, "generate a key") \
    COMMAND(keys, help, "print a detailed help message for the manifest command")

#define FOREACH_manifest_COMMAND(COMMAND) \
    COMMAND(manifest, generate, "create a manifest using the input parameters") \
    COMMAND(manifest, validate, "validate the content of the manifest") \
    COMMAND(manifest, print, "print the recognized values of a manifest") \
    COMMAND(manifest, help, "print a detailed help message for the manifest command")

#define FOREACH_configs_COMMAND(COMMAND) \
    COMMAND(configs, print, "print the current configuration") \
    COMMAND(configs, store, "store the current configuration") \
    COMMAND(configs, validate, "validate the current configuration") \
    COMMAND(configs, help, "print a detailed help message for the configs command")

#define FOREACH_help_COMMAND(COMMAND)
/* Still empty */

#define DECLARE_SUBCOMMANDS(category, item, ...) \
    int category##_##item##_command(Context ctx);

#define DECLARE(name, ...) \
    int name##_command(Context ctx); \
    FOREACH_##name##_COMMAND(DECLARE_SUBCOMMANDS)
    
FOREACH_COMMAND(DECLARE);

#undef DECLARE

#define PRINT_HELP(category, name, description) \
    std::cout << "     " << #name << ":\t" << description << std::endl;

#define HELP_FUNCTION(name) \
    int name##_help_command(Context ctx) {\
        FOREACH_##name##_COMMAND(PRINT_HELP) \
        return EXIT_SUCCESS; \
    }

#define ROUTE(category, item, ...) \
        if (subcommand == #item) \
            return category##_##item##_command(ctx);

#define ROUTER_FUNCTION(name) \
    int name##_command(Context ctx) { \
        std::string subcommand = ctx.get_next_command(); \
        FOREACH_##name##_COMMAND(ROUTE) \
        return !( name##_help_command(ctx)); \
    }

#define IMPLEMENT(name) \
    HELP_FUNCTION(name) \
    ROUTER_FUNCTION(name)

/** Utils **/
void print_buf(const unsigned char* data, size_t data_len, size_t offset);
bool write_binary(std::string file, uint8_t* data, size_t data_len);
