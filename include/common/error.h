/** \file error.h
 * \brief Errors definition.
 *
 * Each module should returns just errors of that particular module.
 * In this way it is possible for the calling function to have a
 * finite set of errors returned by the function.
 * \author Antonio Langiu
 * \defgroup Common
 * \brief This module includes common component used in all the library.
 *
 * It includes utilities functions such as the logger and the error
 * functions or the interface for some components that must be defined
 * by the developer.
 * \{
 */
#ifndef ERROR_H_
#define ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GENERATE_ENUM(ENUM) ENUM##_ERROR,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_ERROR(ERROR) \
    /* Generic Errors */    \
    ERROR(GENERIC)          /** Generic error */\
    ERROR(NOT_IMPLEMENTED)  /** Method not implemented */\
    /* Transport Errors */  \
    ERROR(TRANSPORT_INIT)   /** The transport initialization failed */\
    ERROR(CALLBACK)         /** The callback could not be setted */\
    ERROR(RESOLVER)         /** Error resolving the backend */\
    ERROR(INVALID_URL)      /** The URL of the provided resource is invalid */\
    ERROR(BLOCK_WISE)       /** Error during the Block-Wise transfer */\
    ERROR(INVALID_METHOD)   /** The request method is invalid or not supported */\
    ERROR(INVALID_RESOURCE) /** The requested resource is invalid */\
    ERROR(INVALID_DATA)     /** The received data is invalid */\
    ERROR(INVALID_CONN_DATA) /** The data used to inizialize the connection is invalid */\
    ERROR(REQUEST)          /** Error performing the request */\
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
    ERROR(READ_MANIFEST)    \
    /* Memory Objects Errors */ \
    ERROR(GET_NEWEST)       \
    ERROR(GET_OLDEST)       \
    ERROR(COPY_FIRMWARE)    \
    ERROR(INVALIDATE_OBJECT)\
    ERROR(WRITE_MANIFEST)   \
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

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} ERROR_H_ */
