#include <stdlib.h>
#include <stdio.h>

#define SERVER_ADDR "fd11:22::1c62:fd83:83e7:1063"

#include "network.h" // Test header

int main() {
    nTEST_INIT();
    nTEST_RUN(test_conn);
    nTEST_RUN(test_receiver);
    nTEST_END();
    return 0;
}
