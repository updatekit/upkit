#ifndef TEST_PLATFORM_STEST_H_
#define TEST_PLATFORM_STEST_H_

#ifndef PFUNC
#include <stdio.h>
#define PFUNC(...) printf(__VA_ARGS__);
#endif /* PFUNC */

static int test_total = 0;
static int test_passed = 0;
static int test_failed = 0;
static bool cur_test = true;

#ifndef DISABLE_COLORS
#define GREEN(text)     "\x1b[32m"text"\x1b[0m"
#define RED(text)       "\x1b[31m"text"\x1b[0m"
#elif
#define GREEN(text)     text
#define RED(text)       text
#endif

#define PASSED GREEN("passed")
#define FAILED RED("failed")


#define STEST_FORMAT(file, line, message...) { \
    PFUNC("%s:%d", file, line); \
    PFUNC(message); \
}

#define STEST_TRUE_NORET(file, line, cond) { \
    STEST_FORMAT(file, line, ":test: %s\n", (cond)? PASSED:FAILED); \
    cur_test = (cond)? true : false; \
}

#define STEST_TRUE_NORET_MSG(file, line, cond, message...) { \
    STEST_TRUE_NORET(file, line, cond); \
    (cond)?: PFUNC(message); \
}

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define STEST_INIT() { \
    PFUNC("Starting test: %s\n", __FILENAME__); \
} \

#define STEST_END() { \
    PFUNC("===== TEST RESULT =====\n");  \
    PFUNC(" total: %d\n", test_total);   \
    PFUNC(GREEN(" passed: %d")"\n", test_passed); \
    PFUNC(RED(" failed: %d")"\n", test_failed); \
}

#define STEST_DEFINE(name) \
    const size_t name##_line = __LINE__; \
    void name(void) \

#define str(s) #s

#define STEST_RUN(name) { \
    PFUNC("==== running test <"str(name)"> line %d ====\n", name##_line) \
    test_total++; \
    cur_test = true; \
    stest_prepare(); \
    name(); \
    stest_clean(); \
    (cur_test)? test_passed++: test_failed++; \
    PFUNC("==== test <"str(name)">%s ====\n", (cur_test)? PASSED: FAILED); \
}

#define STEST_TRUE(cond) \
    STEST_TRUE_NORET(__FILENAME__, __LINE__, (cond)); \
    if (!(cond)) return;

#define STEST_TRUE_MESSAGE(cond, msg) \
    STEST_TRUE_NORET_MSG(__FILENAME__, __LINE__, (cond), msg); \
    if (!(cond)) return;

#define STEST_FALSE(cond) \
    STEST_TRUE(!(cond));

#define STEST_FALSE_MESSAGE(cond) \
    STEST_TRUE_MESSAGE(!(cond));

#endif /* TEST_PLATFORM_STEST_H_ */
