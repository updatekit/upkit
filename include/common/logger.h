/** \file logger.h
 * \brief Logging functions.
 *
 * 4 logging level are supported:
 *  - Error: Just the errors are printed.
 *  - Warn: Error and warnings are printed.
 *  - Info: Error, warnign and also information strings.
 *  - Debug: All the output is printed. This affect heavily the
 *  memory footprint of the library.
 * \author Antonio Langiu
 * \defgroup Common
 * \{
 */
#ifndef LOGGER_H_
#define LOGGER_H_

#include "common/settings.h"
#include "common/error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The verbosity_level variable indicates the log_impling level. 
 * If you want to change it at runtime you can define the external
 * symbol in your application.
 * In case you don't need to change it at runtime you can define it as
 * a constant, defining the LOGGER_VERBOSITY define. In this way the 
 * compiler will be able to optimize the code and remove the non needed
 * debugging directives.
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

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LOGGER_H_ */
