#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include "contiki.h"

#define TOSTR(token) #token

#if EVALUATE_TIME == 1

#define DEFINE_TIME_EVALUATOR(name) \
    static rtimer_clock_t name;

#define START_TIME_EVALUATOR(name) \
    name = RTIMER_NOW();

// This prints a CSV. To parse you need to remove the first evaluation token
#define STOP_TIME_EVALUATOR(name) \
    name = (RTIMER_NOW() - name);

#define PRINT_TIME_EVALUATOR(type, name) \
    printf("%u;time;" TOSTR(type) ";%lu;%u\n", test_id, name, RTIMER_SECOND);

#else /* EVALUATE_TIME == 1 */

#define DEFINE_TIME_EVALUATOR(name)
#define START_TIME_EVALUATOR(name)
#define STOP_TIME_EVALUATOR(name)
#define PRINT_TIME_EVALUATOR(type, name)

#endif /* EVALUATE_TIME == 1 */


#if EVALUATE_ENERGY == 1

#include "sys/energest.h"

#define DEFINE_ENERGY_EVALUATOR(name) \
    static unsigned long cpu_##name; \
    static unsigned long lmp_##name; \
    static unsigned long deep_lpm_##name; \
    static unsigned long tx_##name; \
    static unsigned long rx_##name;

#define START_ENERGY_EVALUATOR(name) \
    energest_flush(); \
    cpu_##name = (unsigned long)energest_type_time(ENERGEST_TYPE_CPU); \
    lmp_##name = (unsigned long)energest_type_time(ENERGEST_TYPE_LPM); \
    deep_lpm_##name = (unsigned long)energest_type_time(ENERGEST_TYPE_DEEP_LPM); \
    tx_##name = (unsigned long)energest_type_time(ENERGEST_TYPE_TRANSMIT); \
    rx_##name = (unsigned long)energest_type_time(ENERGEST_TYPE_LISTEN); \

#define STOP_ENERGY_EVALUATOR(name) \
    energest_flush(); \
    cpu_##name = (unsigned long)energest_type_time(ENERGEST_TYPE_CPU) - cpu_##name; \
    lmp_##name = (unsigned long)energest_type_time(ENERGEST_TYPE_LPM) - lmp_##name; \
    deep_lpm_##name = (unsigned long)energest_type_time(ENERGEST_TYPE_DEEP_LPM) - deep_lpm_##name; \
    tx_##name = (unsigned long)energest_type_time(ENERGEST_TYPE_TRANSMIT) - tx_##name; \
    rx_##name = (unsigned long)energest_type_time(ENERGEST_TYPE_LISTEN) - rx_##name;

#define PRINT_ENERGY_EVALUATOR(type, name) \
    printf("%u;energy;" TOSTR(type) ";%lu;%lu;%lu;%lu;%lu;%d\n", \
            test_id, cpu_##name, lmp_##name, deep_lpm_##name, tx_##name, rx_##name, ENERGEST_SECOND);


#else /* EVALUATE_ENERGY */

#define DEFINE_ENERGY_EVALUATOR(name)
#define START_ENERGY_EVALUATOR(name)
#define STOP_ENERGY_EVALUATOR(name)
#define PRINT_ENERGY_EVALUATOR(type, name)

#endif /* EVALUATE_ENERGY */

#if EVALUATE_TIME ==1 || EVALUATE_ENERGY == 1

#define INIT_TEST(value) \
    extern version_t test_id;

#define DEFINE_EVALUATOR(name) \
    DEFINE_TIME_EVALUATOR(name); \
    DEFINE_ENERGY_EVALUATOR(name);

#define START_EVALUATOR(name) \
    START_TIME_EVALUATOR(name); \
    START_ENERGY_EVALUATOR(name);

#define STOP_EVALUATOR(name) \
    STOP_TIME_EVALUATOR(name); \
    STOP_ENERGY_EVALUATOR(name);

#define PRINT_EVALUATOR(type, name) \
    PRINT_TIME_EVALUATOR(type, name); \
    PRINT_ENERGY_EVALUATOR(type, name);

#define EVALUATE_AND_RESTART(type, name) \
    STOP_EVALUATOR(name); \
    PRINT_EVALUATOR(type, name); \
    START_EVALUATOR(name);
#else

#define INIT_TEST(value)
#define DEFINE_EVALUATOR(name)
#define START_EVALUATOR(name)
#define STOP_EVALUATOR(name)
#define PRINT_EVALUATOR(type, name)
#define EVALUATE_AND_RESTART(type, name)

#endif

#endif /* EVALUATOR_H_ */
