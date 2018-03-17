#include "common/libpull.h"

#if LOGGER_VERBOSITY >= 2
static const char* error_str[] = {
    "PULL_SUCCESS",
    FOREACH_ERROR(GENERATE_STRING)
}; 
inline const char* err_as_str(pull_error err) {
    return error_str[err];
}
#else 
const char not_implemented[] = "not_implemented";
inline const char* err_as_str(pull_error err) {
    return not_implemented;
}
#endif


