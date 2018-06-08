#ifndef TEST_PLATFORM_SHARED_H_
#define TEST_PLATFORM_SHARED_H_

#include <unity.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DEFINE_TEST(name) \
    const size_t name##_line = __LINE__; \
    void name(void)

#endif /* TEST_PLATFORM_SHARED_H_ */
