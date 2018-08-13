/** \file logger.h
 * \author Antonio Langiu
 * \defgroup com_logger

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
 *
 * 4 logging level are supported:
 *  - Error: Just the errors are printed.
 *  - Warn: Error and warnings are printed.
 *  - Info: Error, warnign and also information strings.
 *  - Debug: All the output is printed. This affect heavily the
 *  memory footprint of the library.
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

#define log_impl(verbosity, ...) \
    if (verbosity <= verbosity_level) { \
        log_output(__VA_ARGS__);}

#if LOGGER_VERBOSITY >= VERBOSITY_DEBUG
    #define log_err(error, ...) \
        log_output("%s:", err_as_str(error)); \
        log_output(__VA_ARGS__);
#else
    #define log_err(error, ...) \
        log_output("%s\n", err_as_str(error));
#endif

#else /* LOGGER_VERBOSITY */
#define log_impl(...)
#define log_err(...)
#endif /* LOGGER_VERBOSITY */

#define log_debug(...) log_impl(VERBOSITY_DEBUG, __VA_ARGS__)
#define log_info(...) log_impl(VERBOSITY_INFO, __VA_ARGS__)
/**
 * \brief This function takes as first parameter an error and
 * then an arbitrary number of arguments that will be printed.
 * The first arguemnt will be used to print the literal value
 * of the error.
 */
#define log_error(err, ...) log_err(err, __VA_ARGS__)
#define log_warn(...) log_impl(VERBOSITY_WARN, __VA_ARGS__)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_LOGGER_H_ */
