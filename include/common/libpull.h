#ifndef LIBPULL_H_
#define LIBPULL_H_

/** This file is used to include all the common header
 * needed by every module of the library */

/** The library requires some configuration symbols to be defined.
 * This can be done passing them directly to the compiler or
 * defining a config.h file (manually or using the configure script)
 * and including its directory in the header directory */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common/external.h"
#include "common/types.h"
#include "common/error.h"
#include "common/callback.h"
#include "common/async.h"
#include "common/logger.h"

#endif /* LIBPULL_H_ */
