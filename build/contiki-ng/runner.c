#include "contiki.h"
#include "contiki-lib.h"
#include "dev/watchdog.h"
#include "button-sensor.h"
#include "net/ipv6/uip-icmp6.h"
#include "leds.h"

PROCESS(main_process, "Main process");
AUTOSTART_PROCESSES(&main_process);

PROCESS_NAME(update_process);
PROCESS_NAME(main_process);

static uip_ipaddr_t server_addr;
static struct uip_icmp6_echo_reply_notification icmp6_notification;
static uint8_t echo_reply = 0;
static struct etimer et;

static void icmp6_handler(uip_ipaddr_t *source, uint8_t ttl, 
        uint8_t *data, uint16_t datalen) {
    echo_reply++;
}

PROCESS_THREAD(main_process, ev, data) {
    PROCESS_BEGIN();

    // (1) Wait for network setup
    uip_icmp6_echo_reply_callback_add(&icmp6_notification, icmp6_handler);
    HARDCODED_PROV_SERVER(&server_addr);
    do {
        uip_icmp6_send(&server_addr, ICMP6_ECHO_REQUEST, 0, 
                        UIP_ICMP6_ECHO_REQUEST_LEN);
        etimer_set(&et, (CLOCK_SECOND/2));
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    } while(!echo_reply);

    // (2) Start the update process
    process_start(&update_process, NULL);

    // (3) Toggle a led until the system reboots with a new update
    do {
        leds_toggle(LEDS_RED);
        etimer_set(&et, (CLOCK_SECOND*1));
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    } while (1);
    PROCESS_END();
}
