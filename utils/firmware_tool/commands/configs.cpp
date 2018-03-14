#include "commands.hpp"
#include "context.hpp"
#include <fstream>

IMPLEMENT(configs)


bool validate_platform(std::string value) {
    return true; // TODO find a way to validate them
}
bool validate_version(std::string value) {
    return true; // TODO find a way to validate them
}
bool validate_offset(std::string value) {
    return true; // TODO find a way to validate them
}
bool validate_signature_file(std::string value) {
    return file_exists(value);
}
bool validate_vendor_priv_key(std::string value) {
    return file_exists(value);
}
bool validate_vendor_pub_key(std::string value) {
    return file_exists(value);
}
bool validate_server_priv_key(std::string value) {
    return file_exists(value);
}
bool validate_server_pub_key(std::string value) {
    return file_exists(value);
}
bool validate_binary_file(std::string value) {
    return file_exists(value);
}
bool validate_out_file(std::string value) {
    return file_exists(value);
}

#define PRINT_ARG_VALIDITY(type, name, ...) \
    std::cout << std::left << " " << std::setw(20) << #name  \
    << std::setw(20) << std::left << ctx.get_##name() << " [" \
    << (validate_##name(ctx.get_##name())? "":"in") << "valid]" << std::endl;
int configs_validate_command(Context ctx) {
    FOREACH_ARG(PRINT_ARG_VALIDITY);
    return 0;
}

int configs_print_command(Context ctx) {
    ctx.print_arguments();
    return 0;
}
