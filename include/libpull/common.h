#ifndef LIBPULL_COMMON_H_
#define LIBPULL_COMMON_H_

/** This file is used to include all the common header
 * needed by every module of the library */

/** The library requires some configuration symbols to be defined.
 * This can be done passing them directly to the compiler or
 * defining a config.h file (manually or using the configure script)
 * and including its directory in the header directory */
#ifdef HAVE_CONFIG_H
#include "libpull_config.h"
#endif /* HAVE_CONFIG_H */

#include <libpull/common/types.h>
#include <libpull/common/error.h>
#include <libpull/common/callback.h>
#include <libpull/common/logger.h>
#include <libpull/common/pull_assert.h>
#include <libpull/common/external.h>

#endif /* LIBPULL_COMMON_H_ */
