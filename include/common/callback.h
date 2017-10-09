#ifndef _CALLBACK_H
#define _CALLBACK_H

#include "common/error.h"

typedef void (* callback)(pull_error err, const char* data, int len, void* more);

#endif
