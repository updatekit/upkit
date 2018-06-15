#include <libpull/security.h>
#include "default_keys.h"

#include <stdio.h>
#include <string.h>

#include "security.h"

int main(void) {
    UnityBegin(__FILENAME__);
    RUN_TEST(test_digest_functions, test_digest_functions_line);
    RUN_TEST(test_ecc_functions, test_ecc_functions_line);
    RUN_TEST(test_security, test_security_line);
    return UnityEnd();
}
