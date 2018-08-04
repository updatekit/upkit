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
#ifndef LIBPULL_COMMON_LOGGER_H_
#define LIBPULL_COMMON_LOGGER_H_

#include <libpull/common/error.h>

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
#include <stdint.h>
#ifdef LOGGER_VERBOSITY
static const uint8_t verbosity_level = LOGGER_VERBOSITY;
#else
extern uint8_t verbosity_level;
#endif

#define VERBOSITY_ERROR 1
#define VERBOSITY_WARN 2
#define VERBOSITY_INFO 3
#define VERBOSITY_DEBUG 4

#include <stdio.h>
#define log_output(...) printf(__VA_ARGS__)

#if LOGGER_VERBOSITY > 0

#define log_impl(verbosity, args...) \
    if (verbosity <= verbosity_level) { \
        log_output(args);}

#if LOGGER_VERBOSITY >= VERBOSITY_DEBUG
    #define log_err(error, args...) \
        log_output("%s:", err_as_str(error)); \
        log_output(args);
#else
    #define log_err(error, args...) \
        log_output("%s\n", err_as_str(error));
#endif

#else /* LOGGER_VERBOSITY */
#define log_impl(...)
#define log_err(...)
#endif /* LOGGER_VERBOSITY */

#define log_debug(args...) log_impl(VERBOSITY_DEBUG, args)
#define log_info(args...) log_impl(VERBOSITY_INFO, args)
#define log_error(err, args...) log_err(err, args)
#define log_warn(args...) log_impl(VERBOSITY_WARN, args)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_LOGGER_H_ */
