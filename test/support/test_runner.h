#ifndef TEST_SUPPORT_TEST_RUNNER_H_
#define TEST_SUPPORT_TEST_RUNNER_H_

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

void setUp(void);
void tearDown(void);

#define IMPLEMENT_RUNNER(test_func, test_line) \
{ \
  Unity.CurrentTestName = #test_func; \
  Unity.CurrentTestLineNumber = test_line; \
  Unity.NumberOfTests++; \
  if (TEST_PROTECT()){ \
      setUp(); \
      test_##test_func(); \
  } \
  if (TEST_PROTECT()){ \
    tearDown(); \
  } \
  UnityConcludeTest(); \
}

void resetTest(void) {
  tearDown();
  setUp();
}

#define DEFINE_MAIN() \
    int main(void) { \
        UnityBegin(__FILENAME__); \
        FOREACH_TEST(IMPLEMENT_RUNNER); \
        return UnityEnd(); \
	}

#define DEFINE_RUNNER(test_func, test_line) \
    void test_##test_func(void);

#define TEST_RUNNER() \
    FOREACH_TEST(DEFINE_RUNNER) \
    DEFINE_MAIN()

#endif /* TEST_SUPPORT_TEST_RUNNER_H_ */
