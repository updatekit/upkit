#include "common/settings.h"

#ifndef __COMMON_ERROR_H
#define __COMMON_ERROR_H

#define GENERATE_ENUM(ENUM) ENUM##_ERROR,
#define GENERATE_STRING(STRING) #STRING,


/* Each module should returns just errors of that particular module.
 * In this way it is possible for the calling function to have a
 * finite set of errors returned by the function
 */
#define FOREACH_ERROR(ERROR) \
    /* Generic Errors */    \
    ERROR(GENERIC)          \
    ERROR(NOT_IMPLEMENTED)  \
    /* Transport Errors */  \
    ERROR(TRANSPORT_INIT)   \
    ERROR(CALLBACK)         \
    ERROR(RESOLVER)         \
    ERROR(INVALID_URL)      \
    ERROR(BLOCK_WISE)       \
    ERROR(INVALID_METHOD)   \
    ERROR(INVALID_RESOURCE) \
    ERROR(INVALID_DATA)     \
    ERROR(INVALID_CONN_DATA) \
    ERROR(REQUEST)          \
    ERROR(REQUEST_RST)      \
    ERROR(SEND)             \
    /* Memory Errors */     \
    ERROR(MEMORY)           \
    ERROR(INVALID_OBJECT)   \
    ERROR(INVALID_ACCESS)   \
    ERROR(MEMORY_MAPPING)   \
    ERROR(MEMORY_OPEN)      \
    ERROR(MEMORY_CLOSE)     \
    ERROR(MEMORY_ERASE)     \
    ERROR(MEMORY_READ)      \
    ERROR(MEMORY_WRITE)     \
    ERROR(MEMORY_FLUSH)     \
    ERROR(READ_METADATA)    \
    /* Memory Objects Errors */ \
    ERROR(GET_NEWEST)       \
    ERROR(GET_OLDEST)       \
    ERROR(COPY_FIRMWARE)    \
    ERROR(INVALIDATE_OBJECT)\
    ERROR(WRITE_METADATA)   \
    /* Receiver Errors */   \
    ERROR(RECEIVER_OPEN)    \
    ERROR(RECEIVER_CHUNK)   \
    ERROR(RECEIVER_CLOSE)   \
    ERROR(INVALID_SIZE)     \
    ERROR(NETWORK)          \
    /* Subscriber Errors */ \
    ERROR(SUBSCRIBE)        \
    ERROR(SUBSCRIBER_CHECK) \
    /* Digest Errors */     \
    ERROR(DIGEST_INIT)      \
    ERROR(DIGEST_UPDATE)    \
    ERROR(DIGEST_FINAL)     \
    ERROR(SHA256_INIT)      \
    ERROR(SHA256_UPDATE)    \
    ERROR(SHA256_FINAL)     \
    /* Verification Errors */ \
    ERROR(NOT_SUPPORTED_CURVE) \
    ERROR(VERIFICATION_FAILED) \

typedef enum {
    PULL_SUCCESS = 0,
    FOREACH_ERROR(GENERATE_ENUM)
} pull_error;

const char* err_as_str(pull_error err);

#endif
