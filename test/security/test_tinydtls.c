#include "memory/memory_objects.h"
#include "sample_data.h"
#include "security/digest.h"
#include "security/sha256.h"
#include "security/ecc.h"
#include "security/verifier.h"
#include "common/error.h"

#ifdef WITH_CEEDLING
#include "security/tinydtls.h"
#endif

#include "memory_mock.h" // Mock
#include "memory_file_posix.h" // Real Implmentation

#include "manifest_mock.h" // Mock
#include "memory/simple_manifest.h" // Real Implementation

#include <string.h>

#include "test_runner.h"
#include "unity.h"

DIGEST_FUNC(tinydtls);
digest_func func;

#include "test_verifier.h"

#define FOREACH_TEST(DO) \
    DO(ecc_verify, 0)\
    DO(ecc_verify_invalid_curve, 0)\
    DO(ecc_verify_invalid_signature, 0)\
    DO(sha256, 0)\
    DO(sha256_invalid_init, 0)\
    DO(sha256_invalid_update, 0)\
    DO(sha256_invalid_final, 0)\
    DO(verify_object_valid,0)\
    DO(verify_object_invalid_object,0)\
    DO(verify_object_invalid_read,0)\
    DO(verify_object_invalid_digest_init,0)\
    DO(verify_object_invalid_digest_update,0)\
    DO(verify_object_invalid_key,0)
// DO(sign,0) I need to fix the code
TEST_RUNNER();

void setUp() {
    func = tinydtls_digest_sha256;
    memory_mock_restore();
    manifest_mock_restore();
}
