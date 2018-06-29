#ifndef LIBPULL_PUSH_RECEIVER_H_
#define LIBPULL_PUSH_RECEIVER_H_

#include <libpull/common.h>
#include <libpull/network/writer.h>

#define UPDATE_CMD_VAL     0xab
#define UPDATE_CMD_LEN 1

typedef enum libpull_cmd_t {
    CMD_PACKAGE,
    CMD_UPDATE,
} libpull_cmd_t;

enum libpull_state_t {
    STATE_IDLE = 0,
    STATE_DOWNLOADING = 1,
    STATE_DOWNLOADED = 2,
    STATE_UPDATING = 3
};

enum libpull_result_t {
    RESULT_INIT = 0,
    RESULT_SUCCESS = 1,
    RESULT_IMAGE_TOO_BIG = 2,
    RESULT_OUT_OF_RAM = 3,
    RESULT_CONNECTION_LOST = 4,
    RESULT_INTEGRITY_CHECK = 5,
    RESULT_UNSUPPORTED_PACKAGE = 6,
    RESULT_INVALID_URI = 7,
    RESULT_UPDATE_FAILED = 8,
    RESULT_UNSUPPORTED_PROTOCOL = 9
};

typedef uint8_t libpull_state_t;
typedef uint8_t libpull_result_t;

typedef struct push_receiver_ctx_t {
    version_t version;
    platform_t platform;
    identity_t identity;
    libpull_state_t state;
    libpull_result_t result;
    uint32_t received;
    writer_ctx_t wctx;
    mem_id_t id;
    mem_object_t* obj;
} push_receiver_ctx_t;

pull_error push_receiver_init(push_receiver_ctx_t* ctx, mem_object_t* obj);

pull_error pre_verification(manifest_t* mt, void* user_data);

pull_error post_verification(mem_object_t* obj, void* user_data);

pull_error push_receiver_receive(push_receiver_ctx_t* ctx, libpull_cmd_t cmd, const uint8_t* buf, size_t len);

#endif /* LIBPULL_PUSH_RECEIVER_H_ */
