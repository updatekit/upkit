#include <stdlib.h>
#include <stdio.h>

#define SERVER_ADDR "fd11:22::38d3:2ff5:4d7:bc9d"

#include "network.h" // Test header

int main() {
    nTEST_INIT();
    printk("hey\n");
    nTEST_RUN(test_conn);
    nTEST_END();
    return 0;
}
