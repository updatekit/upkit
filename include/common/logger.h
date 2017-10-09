#include "common/settings.h"
#include "common/error.h"

#ifndef LOGGER_H_
#define LOGGER_H_

/* This variable indicates the log_impling level. If you want to change it
 * at runtime you can define the external symbol in your application.
 * In case you don't want it will be defined as a constant value, allowing
 * the compiler to remove the log_impling functions that are not needed
 * for that log_impling level
 */
#ifdef LOGGER_VERBOSITY
static const uint8_t verbosity_level = LOGGER_VERBOSITY;
#else
extern uint8_t verbosity_level;
#endif

#define VERBOSITY_ERROR 1
#define VERBOSITY_WARN 2
#define VERBOSITY_INFO 3
#define VERBOSITY_DEBUG 4

/*static const char* LOG_STR[] = {
    "ERROR: ",
    "ERROR: ",
    "WARN: ",
    "INFO: ",
    "DEBUG: "
};
*/

#include <stdio.h>
#define log_output(args...) printf(args)

#ifdef LOGGER_VERBOSITY

#define log_impl(verbosity, args...) \
    if (verbosity <= verbosity_level) { \
        log_output(args);}

#define log_err(error, args...) {\
    log_output("ERROR: %s: ",err_as_str(error)); \
    log_output(args);}

#else /* LOGGER_VERBOSITY */
#define log_impl(...)
//#define log_err(verbosity, error, args...) log_output("%s %d\n", LOG_STR[verbosity], error);
#endif /* LOGGER_VERBOSITY */

#define log_debug(args...) log_impl(VERBOSITY_DEBUG, args)
#define log_info(args...) log_impl(VERBOSITY_INFO, args)
#define log_error(err, args...) log_err(err, args)
#define log_warn(args...) log_impl(VERBOSITY_WARN, args)

#endif // LOGGER_H_
