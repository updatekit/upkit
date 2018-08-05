#include <stdlib.h>
#include <stdio.h>

#include "network.h" // Test header

#include "shell.h" // RIOT header
#include "msg.h" // RIOT header
#include "xtimer.h" // RIOT header
#include "thread.h" // RIOT header

static char thread_stack[THREAD_STACKSIZE_DEFAULT*4];
static void* test_thread(void* args) {
    // Wait 4 seconds for the network to setup
    xtimer_sleep(4);
    UnityBegin("network.h");
    RUN_TEST(test_conn, test_conn_line);
    RUN_TEST(test_receiver, test_receiver_line);
    UnityEnd();
    return NULL;
}

/* a sendto() call performs an implicit bind(), hence, a message queue is
 * required for the thread executing the shell */
#define MAIN_MSG_QUEUE_SIZE (4)
static msg_t main_msg_queue[MAIN_MSG_QUEUE_SIZE];
static const shell_command_t shell_commands[] = {
    { NULL, NULL, NULL }
};

int main(void) {
    msg_init_queue(main_msg_queue, MAIN_MSG_QUEUE_SIZE);

    puts("Initialize testing thread");
    if (thread_create(thread_stack, sizeof(thread_stack), 
                THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                test_thread, NULL, "testing server") <= KERNEL_PID_UNDEF) {
        puts("Error initializing thread");
        return 1;
    }
    puts("Running shell...");
    puts("You can use the shell to test the network connectivity (ifconfig, nid route, ping6)...");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    // shell_run is a blocking function! You should nerver be here!
    return 0;
}

