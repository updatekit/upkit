/* include/common/libpull_config.h.  Generated from libpull_config.h.in by configure.  */
#ifndef LIBPULL_COMMON_CONFIG_H_
#define LIBPULL_COMMON_CONFIG_H_

/* Use the simple manifest. */
#ifndef SIMPLE_MANIFEST
#define SIMPLE_MANIFEST 1
#endif

/* Use tinycrypt as a cryptographic library. */
#ifndef WITH_TINYCRYPT
#define WITH_TINYCRYPT 1
#endif

/* Use TinyDTLS as a cryptographic library. */
#ifndef WITH_TINYDTLS
#define WITH_TINYDTLS 1
#endif

/* Define logger verbosity */
#ifndef LOGGER_VERBOSITY
#define LOGGER_VERBOSITY 4
#endif

#endif /* LIBPULL_COMMON_CONFIG_H_ */
