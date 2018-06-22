#ifndef _nTEST_H_
#define _nTEST_H_

#include <inttypes.h>
#include <string.h>

#ifndef PFUNC
#include <stdio.h>
#define PFUNC(...) printf(__VA_ARGS__)
#endif /* PFUNC */

static int test_passed = 0;
static int test_failed = 0;
static int assertions = 0;
static char* cur_name;

#ifndef COLOR
#define COLOR(text) text
#endif /* COLORS */
#define GREEN(text)     COLOR("\x1b[32m") text COLOR("\x1b[0m")
#define RED(text)       COLOR("\x1b[31m") text COLOR("\x1b[0m")

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define STR(s) #s

#define nTEST_INIT() \
    PFUNC("Starting nTEST testing framework\n")

#define nTEST_END() { \
    PFUNC("========== TEST RESULTS ==========\n"); \
    PFUNC(GREEN(" Passed: %d")" - Total: %d - "RED("Failed: %d")"\n", \
            test_passed, test_passed+test_failed, test_failed); \
}

#define nTEST_RUN(name) { \
    cur_name = STR(name); \
    PFUNC("+-- TEST "STR(name)": running\n"); \
    assertions = 1; \
    ntest_prepare(); \
    name(); \
    ntest_clean(); \
    (assertions)? test_passed++: test_failed++; \
    PFUNC("+-- TEST "STR(name)": %s\n", (assertions)? GREEN("passed"): RED("failed")); \
}

#define nTEST_TRUE(cond, message...) { \
    PFUNC("|---- %s:%s:%d: %s", __FILENAME__, cur_name, __LINE__, \
            (!!((cond)))? GREEN("passed"):RED("failed")); \
    (!!(cond))? PFUNC("\n"): PFUNC(""message); \
    assertions = (!!(cond))? assertions+1: 0; \
    if (!cond) return; \
}

#define nTEST_FALSE(cond, message...) \
    nTEST_TRUE( (!(cond)) , ""message)

#define nTEST_NOT_NULL(ptr, message...) \
    nTEST_TRUE((ptr != NULL), ""message)

#define nTEST_COMPARE_FORMAT(exp, giv, format, cast, message...) { \
    nTEST_TRUE( ( cast (exp) == cast (giv) ), "Expected: "format" Given: "format, cast exp, cast giv) \
    if (!(cast (exp) == cast (giv))) PFUNC(""message); \
}

#define nTEST_COMPARE(exp, giv, message...) \
    nTEST_TRUE( ((exp) == (giv)), "The compared data differs. "message)

#define nTEST_COMPARE_INT(exp, giv, message...) \
    nTEST_COMPARE_FORMAT((exp), (giv), "%"PRIi64, (int64_t), "")

#define nTEST_COMPARE_UINT(exp, giv, message...) \
    nTEST_COMPARE_FORMAT((exp), (giv), "%"PRIu64, (uint64_t), ""message)

#define nTEST_COMPARE_FLOAT(exp, giv, message...) \
    nTEST_COMPARE_FORMAT((exp), (giv), "%f", , ""message)

#define nTEST_COMPARE_DOUBLE(exp, giv, message...) \
    nTEST_COMPARE_FORMAT((exp), (giv), "%f", , ""message)

#define nTEST_COMPARE_HEX(exp, giv, message...) \
    nTEST_COMPARE_FORMAT((exp), (giv), "0x%"PRIx64, (uint64_t), ""message)

#define nTEST_COMPARE_PTR(exp, giv, message...) \
    nTEST_COMPARE_FORMAT((exp), (giv), "0x%"PRIx64, (void*), ""message)

#define nTEST_COMPARE_STR(exp, giv, message...) \
    nTEST_TRUE((strcmp((exp), (giv)) == 0), "The strings differs. "message)

#define nTEST_COMPARE_MEM(s1, s2, n, message...) \
    nTEST_TRUE((memcmp(s1, s2, n) == 0), "The memory differs. "message)

#endif /* nTEST_H_ */

