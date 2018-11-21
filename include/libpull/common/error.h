/** \file error.h
 * \author Antonio Langiu
 * \defgroup com_error
 * \brief Errors definition.
 * \{
 */
#ifndef LIBPULL_COMMON_ERROR_H_
#define LIBPULL_COMMON_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GENERATE_ENUM(ENUM) ENUM##_ERROR,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_ERROR(ERROR) \
    /* Generic Errors */     \
    /** Generic error */     \
    ERROR(GENERIC)           \
    /** Method not implemented */\
    ERROR(NOT_IMPLEMENTED)   \
    /* connection Errors */  \
    ERROR(INVALID_ARGUMENTS) \
    /** The connection initialization failed */\
    ERROR(CONNECTION_INIT)   \
    /** The callback could not be setted */\
    ERROR(CALLBACK)          \
    /** Error resolving the backend */\
    ERROR(RESOLVER)          \
    /** The URL of the provided resource is invalid */\
    ERROR(INVALID_URL)       \
    /** Error during the Block-Wise transfer */\
    ERROR(BLOCK_WISE)        \
    /** The request method is invalid or not supported */\
    ERROR(INVALID_METHOD)    \
    /** The requested resource is invalid */\
    ERROR(INVALID_RESOURCE)  \
    /** The received data is invalid */\
    ERROR(INVALID_DATA)      \
    /** The data used to inizialize the connection is invalid */\
    ERROR(INVALID_CONN_DATA) \
    /** Error performing the request */\
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
    ERROR(READ_MANIFEST)    \
    /* Memory Objects Errors */ \
    ERROR(GET_NEWEST)       \
    ERROR(GET_OLDEST)       \
    ERROR(COPY_FIRMWARE)    \
    ERROR(INVALIDATE_OBJECT)\
    ERROR(WRITE_MANIFEST)   \
    /* Manifeste */         \
    ERROR(INVALID_MANIFEST) \
    /* Receiver Errors */   \
    ERROR(RECEIVER_OPEN)    \
    ERROR(RECEIVER_CHUNK)   \
    ERROR(RECEIVER_CLOSE)   \
    ERROR(INVALID_SIZE)     \
    ERROR(INVALID_IDENTITY) \
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
    /* Pipeline Errors */ \
    ERROR(PIPELINE) \
    /* FSM errors */ \
    ERROR(INVALID_VERSION) \
    ERROR(INVALID_SIGNATURE) \
    ERROR(INVALID_STATE) \
    ERROR(SIGN_FAILED) \

/** 
 * \brief Each module should returns just errors of that particular module.
 * In this way it is possible for the calling function to have a
 * finite set of errors returned by the function.
 */
typedef enum {
    PULL_SUCCESS = 0,
    FOREACH_ERROR(GENERATE_ENUM)
} pull_error;

/** 
 * \brief  This function returns a string representing the literal
 * representation of the error.
 * \param err The error to be printed.
 * \returns Pointer to a string describing the error.
 */
const char* err_as_str(pull_error err);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_ERROR_H_ */
