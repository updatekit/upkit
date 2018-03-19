#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#if EVALUATE_TIME == 1

#define INIT_TEST(value) \
    static version_t test_id = value;

#define DEFINE_EVALUATOR(name) \
    static rtimer_clock_t name;

#define START_TEST \
    printf("== TEST %x == SECLIB=" SEC_LIB_STR " CONN_TYPE=" CONN_TYPE_STR " ==\n", test_id);

#define START_EVALUATOR(name) \
    name = RTIMER_NOW();

#define TOSTR(token) #token
// This prints a CSV. To parse you need to remove the first evaluation token
#define EVALUATE(type, name) \
    name = (RTIMER_NOW() - name); \
    printf("EVALUATION:\t" TOSTR(type) ",%u,%lu,%u\n", test_id, name, RTIMER_SECOND);

#define EVALUATE_AND_RESTART(type, name) \
    EVALUATE(type, name); \
    START_EVALUATOR(name);

#else /* EVALUATE_TIME == 1 */

#define INIT_TEST(value)
#define DEFINE_EVALUATOR(name)
#define START_TEST
#define START_EVALUATOR(name)
#define EVALUATE(type, name)
#define EVALUATE_AND_RESTART(type, name)

#endif /* EVALUATE_TIME == 1 */

#endif /* EVALUATOR_H_ */
