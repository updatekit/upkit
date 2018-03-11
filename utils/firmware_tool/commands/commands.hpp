#include "../context.hpp"

int keys_command(Context ctx);
int manifest_command(Context ctx);
int configs_command(Context ctx);

/*** Utils */
void print_buf(const unsigned char* data, size_t data_len, size_t offset);
